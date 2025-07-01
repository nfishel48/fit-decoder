defmodule FitDecoder.FieldDemo do
  @moduledoc """
  Demo module to load a FIT file and show what data is present.

  Usage in IEx:
      iex> FitDecoder.FieldDemo.run()
      iex> FitDecoder.FieldDemo.run("/path/to/your/file.fit")
  """

  def run(file_path \\ nil) do
    IO.puts("=== FIT File Data Demo ===\n")

    case load_fit_file(file_path) do
      {:ok, fit_data, path} ->
        IO.puts("✓ FIT file loaded successfully from: #{path}")
        show_data(fit_data)

      {:error, reason} ->
        IO.puts("✗ Could not load FIT file: #{reason}")
        IO.puts("\nUsage:")
        IO.puts("  FitDecoder.FieldDemo.run(\"/path/to/your/file.fit\")")
        IO.puts("\nOr place a file named 'test.fit' in one of these locations:")
        IO.puts("  - /Users/nfishel/Downloads/test.fit")
        IO.puts("  - ./test.fit")
        IO.puts("  - ../test.fit")
    end
  end

  defp load_fit_file(nil) do
    # Try default locations
    test_paths = [
      "/Users/nfishel/Downloads/test.fit",
      "test.fit",
      "../test.fit"
    ]

    case Enum.find_value(test_paths, fn path ->
           if File.exists?(path), do: {path, File.read(path)}, else: nil
         end) do
      nil -> {:error, "No FIT file found in default locations"}
      {path, {:ok, data}} -> {:ok, data, path}
      {_path, {:error, reason}} -> {:error, reason}
    end
  end

  defp load_fit_file(file_path) when is_binary(file_path) do
    case File.read(file_path) do
      {:ok, data} -> {:ok, data, file_path}
      {:error, reason} -> {:error, "Could not read #{file_path}: #{reason}"}
    end
  end

  defp show_data(fit_data) do
    case FitDecoder.decode_fit_file(fit_data) do
      records when is_list(records) and length(records) > 0 ->
        IO.puts("✓ Decoded #{length(records)} records\n")
        analyze_and_show(records)

      [] ->
        IO.puts("✗ No records found in FIT file")

      error ->
        IO.puts("✗ Decode error: #{inspect(error)}")
    end
  end

  defp analyze_and_show(records) do
    # Find all unique fields across all records
    all_fields =
      records
      |> Enum.flat_map(&Map.keys/1)
      |> Enum.uniq()
      |> Enum.sort()

    IO.puts("=== AVAILABLE FIELDS ===")
    IO.puts("#{length(all_fields)} different fields found:\n")

    # Show field availability
    Enum.each(all_fields, fn field ->
      count = Enum.count(records, &Map.has_key?(&1, field))
      percentage = Float.round(count / length(records) * 100, 1)
      IO.puts("#{field}: #{count}/#{length(records)} records (#{percentage}%)")
    end)

    IO.puts("\n=== SAMPLE RECORD DATA ===")
    IO.puts("First record with data:\n")

    # Show first record's actual values
    first_record = hd(records)

    first_record
    |> Enum.sort_by(fn {key, _} -> Atom.to_string(key) end)
    |> Enum.each(fn {key, value} ->
      formatted_value = format_value(value)
      IO.puts("#{key}: #{formatted_value}")
    end)

    # Show some statistics for numeric fields
    show_statistics(records, all_fields)
  end

  defp format_value(value) when is_float(value), do: Float.round(value, 3)
  defp format_value(value), do: value

  defp show_statistics(records, fields) do
    IO.puts("\n=== FIELD STATISTICS ===")

    numeric_fields = [:distance, :speed, :heart_rate, :power, :altitude, :cadence, :temperature]

    available_numeric =
      Enum.filter(numeric_fields, fn field ->
        field in fields and Enum.any?(records, &Map.has_key?(&1, field))
      end)

    if length(available_numeric) > 0 do
      Enum.each(available_numeric, fn field ->
        values =
          records
          |> Enum.filter(&Map.has_key?(&1, field))
          |> Enum.map(&Map.get(&1, field))
          |> Enum.filter(&is_number/1)

        if length(values) > 0 do
          min_val = Enum.min(values)
          max_val = Enum.max(values)
          avg_val = Enum.sum(values) / length(values)

          IO.puts(
            "#{field}: min=#{format_value(min_val)}, max=#{format_value(max_val)}, avg=#{format_value(avg_val)}"
          )
        end
      end)
    else
      IO.puts("No common numeric fields found for statistics")
    end

    IO.puts("\n=== SUMMARY ===")
    IO.puts("Total records: #{length(records)}")
    IO.puts("Unique fields: #{length(fields)}")
    IO.puts("Time span: #{get_time_span(records)}")
  end

  defp get_time_span(records) do
    timestamps =
      records
      |> Enum.map(&Map.get(&1, :timestamp))
      |> Enum.filter(&is_integer/1)

    if length(timestamps) > 1 do
      min_time = Enum.min(timestamps)
      max_time = Enum.max(timestamps)
      duration = max_time - min_time

      hours = div(duration, 3600)
      minutes = div(rem(duration, 3600), 60)
      seconds = rem(duration, 60)

      "#{hours}h #{minutes}m #{seconds}s"
    else
      "Unknown"
    end
  end

  @doc """
  Convenience function to show just the available fields in a FIT file
  """
  def show_fields(file_path \\ nil) do
    case load_fit_file(file_path) do
      {:ok, fit_data, _path} ->
        case FitDecoder.decode_fit_file(fit_data) do
          records when is_list(records) and length(records) > 0 ->
            all_fields =
              records
              |> Enum.flat_map(&Map.keys/1)
              |> Enum.uniq()
              |> Enum.sort()

            IO.puts("Available fields (#{length(all_fields)}):")
            Enum.each(all_fields, &IO.puts("  #{&1}"))
            all_fields

          _ ->
            IO.puts("No records found")
            []
        end

      {:error, reason} ->
        IO.puts("Error: #{reason}")
        []
    end
  end

  @doc """
  Get field statistics for a specific field
  """
  def field_stats(field_name, file_path \\ nil) when is_atom(field_name) do
    case load_fit_file(file_path) do
      {:ok, fit_data, _path} ->
        case FitDecoder.decode_fit_file(fit_data) do
          records when is_list(records) and length(records) > 0 ->
            values =
              records
              |> Enum.filter(&Map.has_key?(&1, field_name))
              |> Enum.map(&Map.get(&1, field_name))
              |> Enum.filter(&is_number/1)

            if length(values) > 0 do
              stats = %{
                count: length(values),
                min: Enum.min(values),
                max: Enum.max(values),
                avg: Enum.sum(values) / length(values)
              }

              IO.puts("Statistics for #{field_name}:")
              IO.puts("  Count: #{stats.count}")
              IO.puts("  Min: #{format_value(stats.min)}")
              IO.puts("  Max: #{format_value(stats.max)}")
              IO.puts("  Average: #{format_value(stats.avg)}")
              stats
            else
              IO.puts("No numeric data found for field: #{field_name}")
              nil
            end

          _ ->
            IO.puts("No records found")
            nil
        end

      {:error, reason} ->
        IO.puts("Error: #{reason}")
        nil
    end
  end
end
