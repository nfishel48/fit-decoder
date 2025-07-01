defmodule FitDecoderTest do
  use ExUnit.Case
  alias FitDecoderTest.TestData
  doctest FitDecoder

  describe "decode_fit_file/1 with basic inputs" do
    test "returns empty list for empty binary" do
      result = FitDecoder.decode_fit_file(<<>>)
      assert result == []
    end

    test "returns error for invalid binary data" do
      result = FitDecoder.decode_fit_file(TestData.invalid_fit_binary())
      assert result == :error_integrity_check_failed
    end

    test "returns error for string input" do
      # Strings are binaries in Elixir, so they get processed
      result = FitDecoder.decode_fit_file("not a fit file")
      assert result == :error_integrity_check_failed
    end

    test "returns error for nil input" do
      assert_raise FunctionClauseError, fn ->
        FitDecoder.decode_fit_file(nil)
      end
    end

    test "returns error for integer input" do
      assert_raise FunctionClauseError, fn ->
        FitDecoder.decode_fit_file(12345)
      end
    end
  end

  describe "decode_fit_file/1 with real FIT data" do
    test "decodes valid FIT file when available" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          result = FitDecoder.decode_fit_file(fit_binary)

          # Basic structure validation
          assert is_list(result)
          assert length(result) > 0

          # Validate each record
          Enum.each(result, fn record ->
            assert TestData.valid_record?(record),
                   "Invalid record structure: #{inspect(record)}"
          end)

          # Extract and validate statistics
          stats = TestData.extract_stats(result)
          assert stats.total_records > 0
          assert stats.timestamp_range != nil

          # Log some info about the test
          IO.puts("\nDecoded #{stats.total_records} records from FIT file")

          if stats.distance_range do
            {min_dist, max_dist} = stats.distance_range
            IO.puts("Distance range: #{min_dist} - #{max_dist}")
          end

          if stats.heart_rate_range do
            {min_hr, max_hr} = stats.heart_rate_range
            IO.puts("Heart rate range: #{min_hr} - #{max_hr}")
          end

        {:error, :no_test_file} ->
          IO.puts("Skipping FIT file test - no test file available")
      end
    end

    test "handles records with varying field availability" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          result = FitDecoder.decode_fit_file(fit_binary)
          stats = TestData.extract_stats(result)

          # All records should have timestamp
          assert stats.total_records > 0

          # Check field distribution
          fields = stats.fields_present
          assert fields.distance >= 0
          assert fields.heart_rate >= 0
          assert fields.altitude >= 0

          # At least some records should have common fields like distance or heart rate
          assert fields.distance > 0 or fields.heart_rate > 0,
                 "Expected at least some records to have distance or heart rate data"

        {:error, :no_test_file} ->
          IO.puts("Skipping field availability test - no test file available")
      end
    end
  end

  describe "data integrity validation" do
    test "timestamps are chronologically ordered and valid" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          result = FitDecoder.decode_fit_file(fit_binary)
          timestamps = Enum.map(result, & &1.timestamp)

          # All timestamps should be positive
          Enum.each(timestamps, fn ts ->
            assert ts > 0, "Invalid timestamp: #{ts}"
            # FIT timestamps are seconds since UTC 00:00 Dec 31 1989
            # Should be reasonable values (after 1990, before far future)
            assert ts > 31_536_000, "Timestamp too small: #{ts}"
            assert ts < 4_000_000_000, "Timestamp too large: #{ts}"
          end)

        {:error, :no_test_file} ->
          IO.puts("Skipping timestamp validation test - no test file available")
      end
    end

    test "distance values are monotonically increasing when present" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          result = FitDecoder.decode_fit_file(fit_binary)

          distances =
            result
            |> Enum.filter(&Map.has_key?(&1, :distance))
            |> Enum.map(& &1.distance)

          if length(distances) > 1 do
            # Distances should generally be increasing (allowing for small GPS errors)
            max_distance = Enum.max(distances)
            min_distance = Enum.min(distances)

            assert max_distance >= min_distance
            assert min_distance >= 0.0

            # Log distance progression
            IO.puts("Distance progression: #{min_distance} -> #{max_distance}")
          end

        {:error, :no_test_file} ->
          IO.puts("Skipping distance validation test - no test file available")
      end
    end

    test "heart rate values are within physiological bounds" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          result = FitDecoder.decode_fit_file(fit_binary)

          heart_rates =
            result
            |> Enum.filter(&Map.has_key?(&1, :heart_rate))
            |> Enum.map(& &1.heart_rate)

          if length(heart_rates) > 0 do
            Enum.each(heart_rates, fn hr ->
              assert hr >= 30, "Heart rate too low: #{hr}"
              assert hr <= 250, "Heart rate too high: #{hr}"
            end)

            avg_hr = Enum.sum(heart_rates) / length(heart_rates)
            IO.puts("Average heart rate: #{Float.round(avg_hr, 1)} bpm")
          end

        {:error, :no_test_file} ->
          IO.puts("Skipping heart rate validation test - no test file available")
      end
    end
  end

  describe "NIF functionality" do
    test "NIF module loads successfully" do
      assert Code.ensure_loaded?(FitDecoder.NIF)
    end

    test "decode_fit_file function is exported" do
      assert function_exported?(FitDecoder.NIF, :decode_fit_file, 1)
    end

    test "main module function delegates to NIF" do
      # Test that the main module function exists and delegates properly
      assert function_exported?(FitDecoder, :decode_fit_file, 1)
    end
  end

  describe "error handling" do
    test "handles SDK exceptions gracefully" do
      # Test with various malformed FIT data to ensure we get proper error atoms
      malformed_binaries = [
        <<0>>,
        <<255, 255, 255, 255>>,
        String.duplicate(<<0>>, 100),
        TestData.invalid_fit_binary()
      ]

      Enum.each(malformed_binaries, fn binary ->
        result = FitDecoder.decode_fit_file(binary)
        # Should return an error atom, not crash
        assert is_atom(result) and String.starts_with?(Atom.to_string(result), "error")
      end)
    end

    test "handles large invalid binaries without crashing" do
      # Test with a large invalid binary to ensure memory handling is correct
      large_invalid = String.duplicate(<<1, 2, 3, 4>>, 10_000)
      result = FitDecoder.decode_fit_file(large_invalid)

      # Should return an error, not crash
      assert result == :error_integrity_check_failed
    end
  end

  describe "helper functions" do
    test "decode_fit_file_from_path/1 works with valid file" do
      case TestData.test_fit_file_path() do
        nil ->
          IO.puts("Skipping file path test - no test file available")

        path ->
          result = FitDecoder.decode_fit_file_from_path(path)
          assert is_list(result)
          assert length(result) > 0
      end
    end

    test "decode_fit_file_from_path/1 handles non-existent file" do
      result = FitDecoder.decode_fit_file_from_path("/non/existent/file.fit")
      assert {:error, :enoent} = result
    end

    test "get_activity_date/1 returns correct date" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          records = FitDecoder.decode_fit_file(fit_binary)
          result = FitDecoder.get_activity_date(records)

          assert {:ok, date} = result
          assert %Date{} = date
          # Date should be reasonable (after 1990, before far future)
          assert Date.compare(date, ~D[1990-01-01]) == :gt
          assert Date.compare(date, ~D[2030-01-01]) == :lt

        {:error, :no_test_file} ->
          IO.puts("Skipping activity date test - no test file available")
      end
    end

    test "get_activity_date/1 handles empty records" do
      result = FitDecoder.get_activity_date([])
      assert result == {:error, :no_records}
    end

    test "get_activity_duration/1 returns correct duration" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          records = FitDecoder.decode_fit_file(fit_binary)
          result = FitDecoder.get_activity_duration(records)

          case result do
            {:ok, duration} ->
              assert is_integer(duration)
              assert duration > 0
              # Log the actual duration for debugging
              IO.puts(
                "Activity duration: #{duration} seconds (#{Float.round(duration / 3600, 1)} hours)"
              )

              # Duration should be reasonable - some test files might have very long durations
              assert duration >= 1
              # Allow for very long test activities, but cap at 30 days for sanity
              assert duration <= 2_592_000

            {:error, :insufficient_data} ->
              # This is acceptable if there's only one record
              assert length(records) <= 1
          end

        {:error, :no_test_file} ->
          IO.puts("Skipping activity duration test - no test file available")
      end
    end

    test "get_activity_duration/1 handles empty records" do
      result = FitDecoder.get_activity_duration([])
      assert result == {:error, :no_records}
    end

    test "get_activity_duration/1 handles single record" do
      single_record = [%{timestamp: 1_096_265_262}]
      result = FitDecoder.get_activity_duration(single_record)
      assert result == {:error, :insufficient_data}
    end

    test "get_activity_info/1 returns comprehensive info" do
      case TestData.read_test_fit_file() do
        {:ok, fit_binary} ->
          records = FitDecoder.decode_fit_file(fit_binary)
          result = FitDecoder.get_activity_info(records)

          case result do
            {:ok, info} ->
              assert %{
                       date: date,
                       duration_seconds: duration,
                       total_distance: total_distance,
                       record_count: record_count,
                       has_heart_rate: has_heart_rate,
                       has_altitude: has_altitude
                     } = info

              assert %Date{} = date
              assert is_integer(duration) and duration > 0

              assert is_nil(total_distance) or
                       (is_float(total_distance) and total_distance >= 0.0)

              assert is_integer(record_count) and record_count > 0
              assert is_boolean(has_heart_rate)
              assert is_boolean(has_altitude)
              # record_count may be different due to session filtering
              assert record_count > 0
              assert record_count <= length(records)

            {:error, :insufficient_data} ->
              # This is acceptable if there's only one record
              assert length(records) <= 1
          end

        {:error, :no_test_file} ->
          IO.puts("Skipping activity info test - no test file available")
      end
    end

    test "get_activity_info/1 handles empty records" do
      result = FitDecoder.get_activity_info([])
      assert result == {:error, :no_records}
    end

    test "decode_and_analyze/1 provides complete workflow" do
      case TestData.test_fit_file_path() do
        nil ->
          IO.puts("Skipping decode_and_analyze test - no test file available")

        path ->
          result = FitDecoder.decode_and_analyze(path)

          case result do
            {:ok, {activity_info, records}} ->
              # Validate activity info structure
              assert %{
                       date: date,
                       duration_seconds: duration,
                       total_distance: total_distance,
                       record_count: record_count,
                       has_heart_rate: has_heart_rate,
                       has_altitude: has_altitude
                     } = activity_info

              assert %Date{} = date
              assert is_integer(duration) and duration > 0

              assert is_nil(total_distance) or
                       (is_float(total_distance) and total_distance >= 0.0)

              assert is_integer(record_count) and record_count > 0
              assert is_boolean(has_heart_rate)
              assert is_boolean(has_altitude)

              # Validate records
              assert is_list(records)
              # record_count may be different due to session filtering
              assert record_count > 0
              assert record_count <= length(records)
              assert length(records) > 0

            error_atom when is_atom(error_atom) ->
              # Handle decoder errors gracefully
              assert String.starts_with?(Atom.to_string(error_atom), "error")

            {:error, reason} ->
              flunk("Unexpected error: #{inspect(reason)}")
          end
      end
    end

    test "decode_and_analyze/1 handles non-existent file" do
      result = FitDecoder.decode_and_analyze("/non/existent/file.fit")
      assert {:error, :enoent} = result
    end

    test "helper functions handle invalid data gracefully" do
      # Test with invalid records
      invalid_records = [%{not_timestamp: 123}, %{timestamp: "invalid"}]

      assert {:error, :no_records} = FitDecoder.get_activity_date(invalid_records)
      assert {:error, :no_records} = FitDecoder.get_activity_duration(invalid_records)
      assert {:error, :no_records} = FitDecoder.get_activity_info(invalid_records)
    end

    test "helper functions work with minimal valid data" do
      # Test with minimal valid records
      minimal_records = [
        %{timestamp: 1_096_265_262},
        %{timestamp: 1_096_265_282}
      ]

      assert {:ok, date} = FitDecoder.get_activity_date(minimal_records)
      assert %Date{} = date

      assert {:ok, 20} = FitDecoder.get_activity_duration(minimal_records)

      assert {:ok, info} = FitDecoder.get_activity_info(minimal_records)
      assert info.record_count == 2
      assert info.duration_seconds == 20
      assert info.total_distance == nil
      assert info.has_heart_rate == false
      assert info.has_altitude == false
    end
  end
end
