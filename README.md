# FitDecoder

FitDecoder is a high-performance Elixir library for decoding Garmin .fit files. It uses a C++ NIF (Native Implemented Function) to wrap the official Garmin FIT C++ SDK, providing a fast and efficient way to extract data from activity files directly within the Elixir ecosystem.

The primary goal of this library is to parse the "Record" messages from a FIT file, which contain moment-by-moment sensor data like GPS coordinates, altitude, heart rate, power, cadence, and many more fields covering everything from basic activity tracking to advanced physiological metrics.

## Requirements

To build and use this library, you will need the following installed on your system (macOS or Linux):

- Elixir (~> 1.15 or newer)
- Erlang/OTP (~> 25.0 or newer)
- A C++ compiler (e.g., g++ or clang++)
- make

## Build Instructions

### 1. Clone the Repository

```bash
git clone <your-repo-url>
cd fit_decoder
```

### 2. Download the Garmin FIT SDK

1. Go to the [FIT SDK download page](https://developer.garmin.com/fit/download/) and download the latest version
2. Unzip the downloaded file
3. Locate the `cpp` directory inside the unzipped folder
4. Copy the entire `cpp` directory into this project's `c_src/fit_sdk/` directory

The final path should be `c_src/fit_sdk/cpp`.

### 3. Fetch Dependencies and Compile

Run the following commands to fetch the Elixir dependencies and compile the project. This will also compile the C++ NIF.

```bash
mix deps.get
mix compile
```

If the compilation is successful, you are ready to use the library.

## Usage

The library provides both low-level decoding functions and high-level helper functions for common workflows. Most application developers will want to use the helper functions for a streamlined experience.

### Quick Demo

To quickly see what data is available in your FIT files, use the built-in demo:

```elixir
# Start IEx
iex -S mix

# Run the demo (will look for test.fit in common locations)
FitDecoder.FieldDemo.run()

# Or specify a specific file
FitDecoder.FieldDemo.run("/path/to/your/activity.fit")

# Just show available fields
FitDecoder.FieldDemo.show_fields("/path/to/your/activity.fit")

# Get statistics for a specific field
FitDecoder.FieldDemo.field_stats(:heart_rate, "/path/to/your/activity.fit")
```

### Recommended Workflow (Helper Functions)

For most applications, use the helper functions that provide a complete workflow:

```elixir
# Complete workflow in one function call
case FitDecoder.decode_and_analyze("/path/to/activity.fit") do
  {:ok, {activity_info, records}} ->
    # Get comprehensive activity information
    IO.puts("Date: #{activity_info.date}")
    IO.puts("Duration: #{activity_info.duration_seconds} seconds") 
    IO.puts("Distance: #{activity_info.total_distance} meters")
    IO.puts("Records: #{activity_info.record_count}")
    IO.puts("Has heart rate: #{activity_info.has_heart_rate}")
    
    # Access individual records for detailed analysis
    Enum.each(records, fn record ->
      if Map.has_key?(record, :heart_rate) do
        IO.puts("HR: #{record.heart_rate} bpm")
      end
    end)
    
  {:error, reason} ->
    IO.puts("Failed to process FIT file: #{reason}")
    
  error_atom when is_atom(error_atom) ->
    IO.puts("FIT decoding error: #{error_atom}")
end
```

### Step-by-Step Workflow

If you prefer more control, use the individual helper functions:

```elixir
# Method 1: From file path
case FitDecoder.decode_fit_file_from_path("/path/to/activity.fit") do
  records when is_list(records) ->
    # Get activity date
    {:ok, date} = FitDecoder.get_activity_date(records)
    IO.puts("Activity date: #{date}")
    
    # Get activity duration
    {:ok, duration} = FitDecoder.get_activity_duration(records)
    IO.puts("Duration: #{duration} seconds")
    
    # Get comprehensive info
    {:ok, info} = FitDecoder.get_activity_info(records)
    IO.inspect(info)
    
  error -> IO.puts("Error: #{error}")
end

# Method 2: From binary data
{:ok, fit_binary} = File.read("/path/to/activity.fit")
records = FitDecoder.decode_fit_file(fit_binary)
{:ok, activity_info} = FitDecoder.get_activity_info(records)
```

### Low-Level Usage

For direct access to the decoder:

```elixir
# Read and decode manually
{:ok, fit_binary} = File.read("/path/to/activity.fit")
records = FitDecoder.decode_fit_file(fit_binary)

IO.puts("Found #{length(records)} records.")
IO.inspect(hd(records), label: "First Record")
```

### Advanced Usage

Access any of the 96+ available fields:

```elixir
# Basic activity data
Enum.each(records, fn record ->
  IO.puts("Time: #{record.timestamp}")
  if Map.has_key?(record, :distance), do: IO.puts("Distance: #{record.distance}m")
  if Map.has_key?(record, :heart_rate), do: IO.puts("HR: #{record.heart_rate} bpm")
end)

# Power analysis (cycling)
power_records = Enum.filter(records, &Map.has_key?(&1, :power))
if length(power_records) > 0 do
  avg_power = power_records |> Enum.map(& &1.power) |> Enum.sum() |> div(length(power_records))
  IO.puts("Average power: #{avg_power}W")
end

# GPS tracking
gps_points = records
|> Enum.filter(fn record ->
  Map.has_key?(record, :position_lat) && Map.has_key?(record, :position_long)
end)
|> Enum.map(fn record ->
  %{
    lat: record.position_lat / :math.pow(2, 31) * 180,
    lng: record.position_long / :math.pow(2, 31) * 180,
    altitude: Map.get(record, :altitude),
    timestamp: record.timestamp
  }
end)

# Running dynamics
running_data = records
|> Enum.filter(&Map.has_key?(&1, :vertical_oscillation))
|> Enum.map(fn record ->
  %{
    cadence: Map.get(record, :cadence),
    vertical_oscillation: record.vertical_oscillation,
    stance_time: Map.get(record, :stance_time),
    step_length: Map.get(record, :step_length)
  }
end)
```

## Return Value

The `decode_fit_file/1` function returns a list of maps. Each map represents a single "Record" message from the FIT file and can contain any of **96+ available fields** depending on your device and activity type.

### Core Fields
Always present when valid:
- `:timestamp` - (Integer) The FIT timestamp for the data point

### Common Fields
- `:altitude` - (Float) Altitude in meters
- `:distance` - (Float) Total distance traveled in meters
- `:heart_rate` - (Integer) Heart rate in beats per minute
- `:speed` - (Float) Speed in meters per second
- `:cadence` - (Integer) Cadence in RPM
- `:power` - (Integer) Power in watts
- `:position_lat` - (Integer) Latitude in semicircles
- `:position_long` - (Integer) Longitude in semicircles

### Advanced Fields
- **Running Dynamics**: `:vertical_oscillation`, `:stance_time`, `:step_length`
- **Cycling Metrics**: `:left_torque_effectiveness`, `:pedal_smoothness`, `:left_right_balance`
- **Physiological**: `:respiration_rate`, `:current_stress`, `:core_temperature`
- **E-bike**: `:battery_soc`, `:motor_power`, `:ebike_assist_mode`
- **Diving**: `:depth`, `:absolute_pressure`, `:air_time_remaining`
- **Blood/Oxygen**: `:saturated_hemoglobin_percent`, `:total_hemoglobin_conc`

### Example Records

Basic cycling record:
```elixir
%{
  timestamp: 978318654,
  altitude: 153.2,
  distance: 5.96,
  heart_rate: 111,
  speed: 4.2,
  power: 185,
  cadence: 87
}
```

Advanced running record:
```elixir
%{
  timestamp: 978318655,
  distance: 1250.5,
  heart_rate: 145,
  speed: 3.8,
  vertical_oscillation: 8.2,
  stance_time: 245.0,
  step_length: 1.45,
  cadence: 180
}
```

GPS-enabled record:
```elixir
%{
  timestamp: 978318656,
  position_lat: 407745893,  # Convert: lat_degrees = value / 2^31 * 180
  position_long: -1221066674,
  altitude: 156.8,
  enhanced_speed: 2.1,
  gps_accuracy: 3
}
```

See [FIELDS.md](FIELDS.md) for complete field documentation.

## Field Availability

Not all fields will be present in every FIT file. Field availability depends on:

1. **Device capabilities** - Only devices with specific sensors can record certain metrics
2. **Activity type** - Swimming fields won't appear in cycling activities  
3. **Recording settings** - Some fields may be disabled to save battery/storage
4. **FIT file version** - Newer fields may not exist in older files

Always check for field presence using `Map.has_key?/2` before accessing values.

## Supported Activity Types

This decoder supports FIT files from various devices and activities:

- **Cycling**: Road, mountain, indoor, e-bike
- **Running**: Road, trail, treadmill, track
- **Swimming**: Pool, open water
- **Fitness**: Gym workouts, strength training
- **Outdoor Activities**: Hiking, skiing, diving
- **Multi-sport**: Triathlon, adventure racing

## Helper Functions

The library provides several helper functions to make common workflows easier:

### Core Functions

- `FitDecoder.decode_fit_file_from_path/1` - Decode directly from file path
- `FitDecoder.get_activity_date/1` - Extract activity start date
- `FitDecoder.get_activity_duration/1` - Calculate activity duration in seconds
- `FitDecoder.get_activity_info/1` - Get comprehensive activity summary
- `FitDecoder.decode_and_analyze/1` - Complete workflow in one call

### Example Helper Function Usage

```elixir
# Get just the activity date
{:ok, records} = FitDecoder.decode_fit_file_from_path("activity.fit")
{:ok, date} = FitDecoder.get_activity_date(records)
# => {:ok, ~D[2024-09-26]}

# Get activity duration
{:ok, duration} = FitDecoder.get_activity_duration(records)  
# => {:ok, 1932}  # seconds

# Get comprehensive activity info
{:ok, info} = FitDecoder.get_activity_info(records)
# => {:ok, %{
#      date: ~D[2024-09-26],
#      duration_seconds: 1932,
#      total_distance: 1985.8,
#      record_count: 387,
#      has_heart_rate: true,
#      has_altitude: false
#    }}
```

### Multi-Session Support

The helper functions automatically detect and handle FIT files containing multiple activity sessions, using the longest continuous session for duration and date calculations.

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed by adding `fit_decoder` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:fit_decoder, "~> 0.1.0"}
  ]
end
```

## Documentation

- [Complete Field Reference](FIELDS.md) - All 96+ available fields with descriptions
- [Field Demo Module](lib/fit_decoder/field_demo.ex) - Interactive exploration of your FIT files

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc) and published on [HexDocs](https://hexdocs.pm). Once published, the docs can be found at <https://hexdocs.pm/fit_decoder>.
