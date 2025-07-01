defmodule FitDecoder do
  @moduledoc """
  A wrapper for the C++ FIT file decoder NIF.

  This module provides functionality to decode FIT (Flexible and Interoperable Data Transfer) files,
  which are commonly used by fitness devices and applications to store workout and activity data.

  ## Examples

      # Decode an empty binary (returns empty list)
      iex> FitDecoder.decode_fit_file(<<>>)
      []

      # Decode invalid binary data (returns error atom)
      iex> FitDecoder.decode_fit_file(<<1, 2, 3, 4>>)
      :error_integrity_check_failed

      # Decode a valid FIT file binary would return a list of maps:
      # [%{timestamp: 1096265262, distance: 0.0, heart_rate: 95}, ...]

  """

  # Define the module that contains the NIF functions.
  # This MUST match the first argument to ERL_NIF_INIT in your C++ code.
  defmodule NIF do
    @on_load :load_nif

    def load_nif do
      nif_file = :filename.join(:code.priv_dir(:fit_decoder), ~c"nif")

      case :erlang.load_nif(nif_file, 0) do
        :ok ->
          :ok

        {:error, {:load_failed, reason}} ->
          IO.puts("Failed to load NIF: #{inspect(reason)}")
          IO.puts("NIF file path: #{nif_file}")
          {:error, {:load_failed, reason}}

        error ->
          IO.puts("Unexpected error loading NIF: #{inspect(error)}")
          IO.puts("NIF file path: #{nif_file}")
          error
      end
    end

    # Define a stub for the NIF.
    # This will be replaced by the actual C++ function when the NIF is loaded.
    def decode_fit_file(_binary), do: :erlang.nif_error(:nif_not_loaded)
  end

  @doc """
  Decodes a FIT file binary and returns a list of maps, where each map
  represents a "Record" message containing sensor data.

  ## Parameters

    * `binary` - A binary containing FIT file data

  ## Returns

    * A list of maps containing decoded record data on success
    * An error atom (`:error_integrity_check_failed`, `:error_sdk_exception`) on failure
    * An empty list if the file contains no record data

  ## Record Fields

  Each record map may contain the following fields (only present if valid data exists):

    * `:timestamp` - Unix timestamp (always present in valid records)
    * `:distance` - Distance in meters (float)
    * `:heart_rate` - Heart rate in beats per minute (integer)
    * `:altitude` - Altitude in meters (float)

  ## Examples

      iex> FitDecoder.decode_fit_file(<<>>)
      []

      iex> FitDecoder.decode_fit_file(<<1, 2, 3, 4>>)
      :error_integrity_check_failed

  """
  def decode_fit_file(binary) when is_binary(binary) do
    NIF.decode_fit_file(binary)
  end

  @doc """
  Decodes a FIT file from a file path and returns the same data as `decode_fit_file/1`.

  ## Parameters

    * `file_path` - Path to a FIT file on disk

  ## Returns

    * Same as `decode_fit_file/1` but reads from file path
    * `{:error, reason}` if file cannot be read

  ## Examples

      iex> FitDecoder.decode_fit_file_from_path("/nonexistent/file.fit")
      {:error, :enoent}

  """
  def decode_fit_file_from_path(file_path) when is_binary(file_path) do
    case File.read(file_path) do
      {:ok, binary} -> decode_fit_file(binary)
      {:error, reason} -> {:error, reason}
    end
  end

  @doc """
  Gets the activity date from decoded FIT file records.

  Returns the date (as a `Date` struct) when the activity started based on the
  earliest timestamp in the records.

  ## Parameters

    * `records` - List of decoded FIT records from `decode_fit_file/1`

  ## Returns

    * `{:ok, %Date{}}` - The date when the activity started
    * `{:error, :no_records}` - If the records list is empty
    * `{:error, :invalid_records}` - If records don't contain valid timestamps

  ## Examples

      iex> records = [%{timestamp: 1745940695}]
      iex> FitDecoder.get_activity_date(records)
      {:ok, ~D[2025-04-29]}

  """
  def get_activity_date(records) when is_list(records) do
    case records do
      [] ->
        {:error, :no_records}

      records ->
        [first | _rest] = records

        case get_record_timestamp(first) do
          {:ok, timestamp} ->
            date = DateTime.from_unix!(timestamp) |> DateTime.to_date()
            {:ok, date}

          {:error, _reason} ->
            {:error, :no_records}
        end
    end
  end

  defp get_record_timestamp(record) when is_map_key(record, :timestamp),
    do: {:ok, record.timestamp}

  defp get_record_timestamp(_record), do: {:error, :key_not_found}

  @doc """
  Gets the activity duration from decoded FIT file records.

  Returns the duration of the activity in seconds based on the difference
  between the earliest and latest timestamps.

  ## Parameters

    * `records` - List of decoded FIT records from `decode_fit_file/1`

  ## Returns

    * `{:ok, duration_seconds}` - Duration in seconds (integer)
    * `{:error, :no_records}` - If the records list is empty
    * `{:error, :invalid_records}` - If records don't contain valid timestamps
    * `{:error, :insufficient_data}` - If there's only one record

  ## Examples

      iex> records = [%{timestamp: 1096265262}, %{timestamp: 1096265322}]
      iex> FitDecoder.get_activity_duration(records)
      {:ok, 60}

  """
  def get_activity_duration(records) when is_list(records) do
    case get_longest_continuous_session(records) do
      {:ok, {start_timestamp, end_timestamp}} when start_timestamp != end_timestamp ->
        {:ok, end_timestamp - start_timestamp}

      {:ok, {_start_timestamp, _end_timestamp}} ->
        {:error, :insufficient_data}

      error ->
        error
    end
  end

  @doc """
  Gets comprehensive activity information from decoded FIT file records.

  This is a convenience function that extracts the most commonly needed
  information from an activity in a single call.

  ## Parameters

    * `records` - List of decoded FIT records from `decode_fit_file/1`

  ## Returns

    * `{:ok, activity_info}` - A map containing activity information
    * `{:error, reason}` - If records are invalid or empty

  The activity_info map contains:
    * `:date` - Date when activity started (`Date` struct)
    * `:duration_seconds` - Duration in seconds (integer)
    * `:total_distance` - Total distance in meters (float, may be nil)
    * `:record_count` - Number of data records (integer)
    * `:has_heart_rate` - Whether heart rate data is present (boolean)
    * `:has_altitude` - Whether altitude data is present (boolean)

  ## Examples

      iex> records = [%{timestamp: 1727321178, distance: 12543.2, heart_rate: 95}, %{timestamp: 1727321238, distance: 12543.2, heart_rate: 98}]
      iex> FitDecoder.get_activity_info(records)
      {:ok, %{
        date: ~D[2024-09-26],
        duration_seconds: 60,
        total_distance: 12543.2,
        record_count: 2,
        has_heart_rate: true,
        has_altitude: false
      }}

  """
  def get_activity_info(records) when is_list(records) do
    with {:ok, date} <- get_activity_date(records),
         {:ok, duration} <- get_activity_duration(records) do
      # Get records from the longest continuous session for more accurate stats
      session_records =
        case get_longest_continuous_session_records(records) do
          {:ok, session_records} -> session_records
          _ -> records
        end

      total_distance = get_total_distance(session_records)

      activity_info = %{
        date: date,
        duration_seconds: duration,
        total_distance: total_distance,
        record_count: length(session_records),
        has_heart_rate: has_field?(session_records, :heart_rate),
        has_altitude: has_field?(session_records, :altitude)
      }

      {:ok, activity_info}
    end
  end

  @doc """
  Convenience function that combines file reading, decoding, and info extraction.

  This function implements the complete workflow: read file → decode → extract info.

  ## Parameters

    * `file_path` - Path to a FIT file on disk

  ## Returns

    * `{:ok, {activity_info, records}}` - Activity info and full record list
    * `{:error, reason}` - If file reading, decoding, or parsing fails
    * Error atoms from the decoder (e.g., `:error_integrity_check_failed`)

  ## Examples

      iex> FitDecoder.decode_and_analyze("/nonexistent/file.fit")
      {:error, :enoent}

  """
  def decode_and_analyze(file_path) when is_binary(file_path) do
    case decode_fit_file_from_path(file_path) do
      records when is_list(records) ->
        case get_activity_info(records) do
          {:ok, activity_info} -> {:ok, {activity_info, records}}
          error -> error
        end

      error_atom when is_atom(error_atom) ->
        error_atom

      {:error, reason} ->
        {:error, reason}
    end
  end

  # Private helper functions

  defp get_longest_continuous_session(records) do
    case find_sessions(records) do
      [] ->
        {:error, :no_records}

      sessions ->
        longest_session =
          sessions
          |> Enum.max_by(fn {start_ts, end_ts, _count} -> end_ts - start_ts end)

        {start_ts, end_ts, _count} = longest_session
        {:ok, {start_ts, end_ts}}
    end
  end

  defp get_longest_continuous_session_records(records) do
    case find_sessions(records) do
      [] ->
        {:error, :no_records}

      sessions ->
        # Find the session with the longest duration
        longest_session =
          sessions
          |> Enum.max_by(fn {start_ts, end_ts, _count} -> end_ts - start_ts end)

        {start_ts, end_ts, _count} = longest_session

        # Filter records to only include those in the longest session
        session_records =
          records
          |> Enum.filter(fn record ->
            ts = Map.get(record, :timestamp)
            is_integer(ts) and ts >= start_ts and ts <= end_ts
          end)

        {:ok, session_records}
    end
  end

  defp find_sessions(records) do
    timestamps =
      records
      |> Enum.map(&Map.get(&1, :timestamp))
      |> Enum.filter(&(is_integer(&1) and &1 > 0))
      |> Enum.sort()

    case timestamps do
      [] ->
        []

      [single_timestamp] ->
        [{single_timestamp, single_timestamp, 1}]

      timestamps ->
        # Group timestamps into sessions based on gaps
        # Consider a gap of more than 1 hour (3600 seconds) as a session break
        gap_threshold = 3600

        {sessions, current_session} =
          timestamps
          |> Enum.reduce({[], {nil, nil, 0}}, fn ts, {sessions, {start_ts, prev_ts, count}} ->
            cond do
              # First timestamp
              start_ts == nil ->
                {sessions, {ts, ts, 1}}

              # Gap detected - finish current session and start new one
              ts - prev_ts > gap_threshold ->
                completed_session = {start_ts, prev_ts, count}
                {[completed_session | sessions], {ts, ts, 1}}

              # Continue current session
              true ->
                {sessions, {start_ts, ts, count + 1}}
            end
          end)

        # Add the final session
        final_sessions = [current_session | sessions]
        Enum.reverse(final_sessions)
    end
  end

  defp get_total_distance(records) do
    distances =
      records
      |> Enum.map(&Map.get(&1, :distance))
      |> Enum.filter(&(is_float(&1) and &1 >= 0.0))

    case distances do
      [] -> nil
      distances -> Enum.max(distances)
    end
  end

  defp has_field?(records, field_name) do
    Enum.any?(records, &Map.has_key?(&1, field_name))
  end
end
