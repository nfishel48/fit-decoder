#!/usr/bin/env elixir

# Example usage of FitDecoder helper functions
# This demonstrates the recommended workflow for application developers

Mix.install([{:fit_decoder, path: "."}])

defmodule FitDecoderExample do
  @moduledoc """
  Example usage of the FitDecoder library with helper functions.

  This demonstrates the complete workflow:
  1. Read and decode a FIT file
  2. Get activity date
  3. Get activity duration
  4. Get comprehensive activity info
  5. Access the raw records for detailed analysis
  """

  def run do
    # Path to your FIT file - update this to point to an actual FIT file
    fit_file_path = "/Users/nfishel/Downloads/test.fit"

    IO.puts("=== FitDecoder Example Usage ===\n")

    # Method 1: Step-by-step workflow
    IO.puts("Method 1: Step-by-step workflow")
    step_by_step_example(fit_file_path)

    IO.puts("\n" <> String.duplicate("=", 50) <> "\n")

    # Method 2: All-in-one workflow
    IO.puts("Method 2: All-in-one workflow (recommended)")
    all_in_one_example(fit_file_path)

    IO.puts("\n" <> String.duplicate("=", 50) <> "\n")

    # Method 3: Working with binary data directly
    IO.puts("Method 3: Working with binary data")
    binary_example(fit_file_path)

    IO.puts("\n=== End of Examples ===")
  end

  defp step_by_step_example(fit_file_path) do
    case File.exists?(fit_file_path) do
      true ->
        IO.puts("📁 Reading FIT file: #{fit_file_path}")

        # Step 1: Decode the FIT file
        case FitDecoder.decode_fit_file_from_path(fit_file_path) do
          records when is_list(records) ->
            IO.puts("✅ Successfully decoded #{length(records)} records")

            # Step 2: Get the activity date
            case FitDecoder.get_activity_date(records) do
              {:ok, date} ->
                IO.puts("📅 Activity date: #{date}")
              {:error, reason} ->
                IO.puts("❌ Failed to get activity date: #{reason}")
            end

            # Step 3: Get the activity duration
            case FitDecoder.get_activity_duration(records) do
              {:ok, duration_seconds} ->
                minutes = div(duration_seconds, 60)
                seconds = rem(duration_seconds, 60)
                IO.puts("⏱️  Activity duration: #{minutes}m #{seconds}s (#{duration_seconds} seconds)")
              {:error, reason} ->
                IO.puts("❌ Failed to get duration: #{reason}")
            end

            # Step 4: Analyze the data
            analyze_records(records)

          error_atom when is_atom(error_atom) ->
            IO.puts("❌ Failed to decode FIT file: #{error_atom}")

          {:error, reason} ->
            IO.puts("❌ Failed to read file: #{reason}")
        end

      false ->
        IO.puts("❌ File not found: #{fit_file_path}")
        IO.puts("   Please update the fit_file_path variable to point to a valid FIT file")
    end
  end

  defp all_in_one_example(fit_file_path) do
    case File.exists?(fit_file_path) do
      true ->
        IO.puts("📁 Processing FIT file: #{fit_file_path}")

        # Single function call for complete workflow
        case FitDecoder.decode_and_analyze(fit_file_path) do
          {:ok, {activity_info, records}} ->
            IO.puts("✅ Successfully processed FIT file!")
            IO.puts("")

            # Display comprehensive activity information
            IO.puts("📊 Activity Summary:")
            IO.puts("   Date: #{activity_info.date}")

            minutes = div(activity_info.duration_seconds, 60)
            seconds = rem(activity_info.duration_seconds, 60)
            IO.puts("   Duration: #{minutes}m #{seconds}s")

            case activity_info.total_distance do
              nil ->
                IO.puts("   Distance: Not available")
              distance ->
                distance_miles = distance * 0.000621371
                IO.puts("   Distance: #{Float.round(distance, 1)}m (#{Float.round(distance_miles, 2)} miles)")
            end

            IO.puts("   Records: #{activity_info.record_count}")
            IO.puts("   Heart Rate Data: #{if activity_info.has_heart_rate, do: "Yes", else: "No"}")
            IO.puts("   Altitude Data: #{if activity_info.has_altitude, do: "Yes", else: "No"}")

            IO.puts("")

            # You still have access to all the raw records for detailed analysis
            analyze_records(records)

          error_atom when is_atom(error_atom) ->
            IO.puts("❌ Failed to decode FIT file: #{error_atom}")

          {:error, reason} ->
            IO.puts("❌ Failed to read file: #{reason}")
        end

      false ->
        IO.puts("❌ File not found: #{fit_file_path}")
        IO.puts("   Please update the fit_file_path variable to point to a valid FIT file")
    end
  end

  defp binary_example(fit_file_path) do
    case File.exists?(fit_file_path) do
      true ->
        IO.puts("📁 Reading FIT file as binary: #{fit_file_path}")

        case File.read(fit_file_path) do
          {:ok, fit_binary} ->
            IO.puts("✅ Read #{byte_size(fit_binary)} bytes")

            # Decode from binary data
            case FitDecoder.decode_fit_file(fit_binary) do
              records when is_list(records) ->
                IO.puts("✅ Successfully decoded #{length(records)} records from binary")

                # Get activity info from records
                case FitDecoder.get_activity_info(records) do
                  {:ok, info} ->
                    IO.puts("📊 Activity from binary: #{info.duration_seconds} seconds on #{info.date}")
                  {:error, reason} ->
                    IO.puts("❌ Failed to get activity info: #{reason}")
                end

              error_atom when is_atom(error_atom) ->
                IO.puts("❌ Failed to decode binary: #{error_atom}")
            end

          {:error, reason} ->
            IO.puts("❌ Failed to read file: #{reason}")
        end

      false ->
        IO.puts("❌ File not found: #{fit_file_path}")
    end
  end

  defp analyze_records(records) do
    IO.puts("\n🔍 Detailed Analysis:")

    # Show sample of first few records
    IO.puts("   First 3 records:")
    records
    |> Enum.take(3)
    |> Enum.with_index(1)
    |> Enum.each(fn {record, idx} ->
      fields =
        record
        |> Map.drop([:timestamp])
        |> Enum.map(fn {k, v} -> "#{k}: #{inspect(v)}" end)
        |> Enum.join(", ")

      IO.puts("     #{idx}. #{fields}")
    end)

    # Statistics
    distances = records |> Enum.map(&Map.get(&1, :distance)) |> Enum.filter(&(&1 != nil))
    heart_rates = records |> Enum.map(&Map.get(&1, :heart_rate)) |> Enum.filter(&(&1 != nil))

    if length(heart_rates) > 0 do
      avg_hr = Enum.sum(heart_rates) / length(heart_rates)
      max_hr = Enum.max(heart_rates)
      min_hr = Enum.min(heart_rates)
      IO.puts("   Heart Rate: #{Float.round(avg_hr, 1)} avg, #{min_hr}-#{max_hr} range")
    end

    if length(distances) > 0 do
      max_distance = Enum.max(distances)
      IO.puts("   Max Distance: #{Float.round(max_distance, 1)}m")
    end
  end
end

# Run the example
FitDecoderExample.run()
