defmodule FitDecoderTest.TestData do
  @moduledoc """
  Test data and utilities for FIT decoder tests.
  """

  @doc """
  Returns the path to a test FIT file if it exists, or nil if not available.
  This allows tests to be conditional on whether test data is available.
  """
  def test_fit_file_path do
    path = "/Users/nfishel/Downloads/test.fit"
    if File.exists?(path), do: path, else: nil
  end

  @doc """
  Reads the test FIT file if it exists, returns {:ok, binary} or {:error, :no_test_file}.
  """
  def read_test_fit_file do
    case test_fit_file_path() do
      nil -> {:error, :no_test_file}
      path -> File.read(path)
    end
  end

  @doc """
  Returns a minimal invalid FIT binary for testing error cases.
  """
  def invalid_fit_binary do
    <<1, 2, 3, 4>>
  end

  @doc """
  Validates that a record has the expected structure and data types.
  """
  def valid_record?(record) do
    is_map(record) and
      Map.has_key?(record, :timestamp) and
      is_integer(record.timestamp) and
      record.timestamp > 0 and
      valid_optional_fields?(record)
  end

  defp valid_optional_fields?(record) do
    valid_distance?(record) and
      valid_heart_rate?(record) and
      valid_altitude?(record)
  end

  defp valid_distance?(record) do
    case Map.get(record, :distance) do
      nil -> true
      distance when is_float(distance) and distance >= 0.0 -> true
      _ -> false
    end
  end

  defp valid_heart_rate?(record) do
    case Map.get(record, :heart_rate) do
      nil -> true
      hr when is_integer(hr) and hr > 0 and hr < 300 -> true
      _ -> false
    end
  end

  defp valid_altitude?(record) do
    case Map.get(record, :altitude) do
      nil -> true
      altitude when is_float(altitude) -> true
      _ -> false
    end
  end

  @doc """
  Extracts statistics from a list of decoded records.
  """
  def extract_stats(records) when is_list(records) do
    timestamps = Enum.map(records, & &1.timestamp)
    distances = records |> Enum.map(&Map.get(&1, :distance)) |> Enum.filter(&(&1 != nil))
    heart_rates = records |> Enum.map(&Map.get(&1, :heart_rate)) |> Enum.filter(&(&1 != nil))
    altitudes = records |> Enum.map(&Map.get(&1, :altitude)) |> Enum.filter(&(&1 != nil))

    %{
      total_records: length(records),
      timestamp_range:
        if(length(timestamps) > 0, do: {Enum.min(timestamps), Enum.max(timestamps)}, else: nil),
      distance_range:
        if(length(distances) > 0, do: {Enum.min(distances), Enum.max(distances)}, else: nil),
      heart_rate_range:
        if(length(heart_rates) > 0, do: {Enum.min(heart_rates), Enum.max(heart_rates)}, else: nil),
      altitude_range:
        if(length(altitudes) > 0, do: {Enum.min(altitudes), Enum.max(altitudes)}, else: nil),
      fields_present: %{
        distance: length(distances),
        heart_rate: length(heart_rates),
        altitude: length(altitudes)
      }
    }
  end
end
