# FitDecoder Helper Functions

This document describes the helper functions added to the FitDecoder library to simplify common workflows for application developers.

## Overview

The helper functions provide a streamlined way to:
1. Decode FIT files from file paths
2. Extract activity date and duration
3. Get comprehensive activity information
4. Handle multi-session FIT files automatically

## Functions

### `decode_fit_file_from_path/1`

Decodes a FIT file directly from a file path.

**Parameters:**
- `file_path` (string) - Path to the FIT file

**Returns:**
- List of decoded records on success
- `{:error, reason}` on file read error
- Error atoms from decoder (e.g., `:error_integrity_check_failed`)

**Example:**
```elixir
case FitDecoder.decode_fit_file_from_path("/path/to/activity.fit") do
  records when is_list(records) ->
    IO.puts("Decoded #{length(records)} records")
  {:error, reason} ->
    IO.puts("File error: #{reason}")
  error_atom ->
    IO.puts("Decoder error: #{error_atom}")
end
```

### `get_activity_date/1`

Extracts the activity start date from decoded records.

**Parameters:**
- `records` - List of decoded FIT records

**Returns:**
- `{:ok, %Date{}}` - Activity start date
- `{:error, :no_records}` - Empty records list
- `{:error, :invalid_records}` - No valid timestamps found

**Example:**
```elixir
{:ok, date} = FitDecoder.get_activity_date(records)
# => {:ok, ~D[2024-09-26]}
```

### `get_activity_duration/1`

Calculates the activity duration from decoded records.

**Parameters:**
- `records` - List of decoded FIT records

**Returns:**
- `{:ok, duration_seconds}` - Duration in seconds (integer)
- `{:error, :no_records}` - Empty records list
- `{:error, :invalid_records}` - No valid timestamps found
- `{:error, :insufficient_data}` - Only one record available

**Example:**
```elixir
{:ok, duration} = FitDecoder.get_activity_duration(records)
# => {:ok, 1932}  # 32 minutes 12 seconds
```

### `get_activity_info/1`

Provides comprehensive activity information in a single call.

**Parameters:**
- `records` - List of decoded FIT records

**Returns:**
- `{:ok, activity_info}` - Map containing activity details
- `{:error, reason}` - If records are invalid or insufficient

**Activity Info Map:**
```elixir
%{
  date: ~D[2024-09-26],           # Activity start date
  duration_seconds: 1932,         # Duration in seconds
  total_distance: 1985.8,         # Total distance in meters (nil if not available)
  record_count: 361,              # Number of records in main session
  has_heart_rate: true,           # Whether heart rate data is present
  has_altitude: false             # Whether altitude data is present
}
```

**Example:**
```elixir
{:ok, info} = FitDecoder.get_activity_info(records)
IO.puts("#{info.duration_seconds} second activity on #{info.date}")
```

### `decode_and_analyze/1`

Complete workflow function that combines file reading, decoding, and analysis.

**Parameters:**
- `file_path` (string) - Path to the FIT file

**Returns:**
- `{:ok, {activity_info, records}}` - Tuple containing activity info and full records
- `{:error, reason}` - File read error
- Error atoms from decoder

**Example:**
```elixir
case FitDecoder.decode_and_analyze("/path/to/activity.fit") do
  {:ok, {activity_info, records}} ->
    IO.puts("Activity: #{activity_info.duration_seconds}s on #{activity_info.date}")
    IO.puts("Records available: #{length(records)}")
    
    # Use records for detailed analysis
    heart_rates = records 
    |> Enum.map(&Map.get(&1, :heart_rate)) 
    |> Enum.filter(&(&1 != nil))
    
  {:error, reason} ->
    IO.puts("Error: #{reason}")
end
```

## Multi-Session Support

The helper functions automatically handle FIT files that contain multiple activity sessions (common in some devices). The functions:

1. **Detect session boundaries** - Identify gaps longer than 1 hour between timestamps
2. **Select longest session** - Use the session with the longest duration for calculations
3. **Provide accurate metrics** - Duration and date reflect the main activity session

This ensures that:
- Duration calculations are reasonable (e.g., 32 minutes instead of 60 days)
- Date reflects the actual activity date
- Statistics are based on the primary activity session

## Error Handling

All helper functions use consistent error handling:

- `{:error, :no_records}` - Empty or missing records
- `{:error, :invalid_records}` - Records without valid timestamps
- `{:error, :insufficient_data}` - Not enough data for calculation
- `{:error, file_reason}` - File system errors (e.g., `:enoent`)
- Error atoms - FIT decoder errors (e.g., `:error_integrity_check_failed`)

## Recommended Usage Patterns

### Pattern 1: Quick Analysis
```elixir
# Single function call for most use cases
{:ok, {info, _records}} = FitDecoder.decode_and_analyze(file_path)
process_activity_summary(info)
```

### Pattern 2: Detailed Analysis
```elixir
# When you need both summary and detailed record access
{:ok, {info, records}} = FitDecoder.decode_and_analyze(file_path)
show_summary(info)
analyze_heart_rate_zones(records)
plot_elevation_profile(records)
```

### Pattern 3: Step-by-Step
```elixir
# When you need granular control
{:ok, records} = FitDecoder.decode_fit_file_from_path(file_path)
{:ok, date} = FitDecoder.get_activity_date(records)
{:ok, duration} = FitDecoder.get_activity_duration(records)
custom_analysis(records, date, duration)
```

## Type Specifications

```elixir
@type record :: %{timestamp: integer(), optional(atom()) => term()}
@type activity_info :: %{
  date: Date.t(),
  duration_seconds: integer(),
  total_distance: float() | nil,
  record_count: integer(),
  has_heart_rate: boolean(),
  has_altitude: boolean()
}

@spec decode_fit_file_from_path(String.t()) :: [record()] | {:error, term()} | atom()
@spec get_activity_date([record()]) :: {:ok, Date.t()} | {:error, atom()}
@spec get_activity_duration([record()]) :: {:ok, integer()} | {:error, atom()}
@spec get_activity_info([record()]) :: {:ok, activity_info()} | {:error, atom()}
@spec decode_and_analyze(String.t()) :: {:ok, {activity_info(), [record()]}} | {:error, term()} | atom()
```
