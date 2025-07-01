# FIT Record Fields Reference

This document lists all the fields available in FIT Record messages that can be extracted using this decoder.

## Overview

Your FIT decoder now supports extracting **96 different fields** from FIT Record messages. These fields cover everything from basic activity data to advanced physiological metrics, diving parameters, and e-bike telemetry.

## Field Categories

### Basic Fields
Always present in records with valid data:

- **`timestamp`** (integer) - Record timestamp in FIT time format
- **`altitude`** (float) - Altitude in meters
- **`distance`** (float) - Cumulative distance in meters  
- **`heart_rate`** (integer) - Heart rate in beats per minute

### Position & Navigation
GPS and movement data:

- **`position_lat`** (integer) - Latitude in semicircles (divide by 2^31 * 180 for degrees)
- **`position_long`** (integer) - Longitude in semicircles (divide by 2^31 * 180 for degrees)
- **`enhanced_altitude`** (float) - High-resolution altitude in meters
- **`speed`** (float) - Speed in meters per second
- **`enhanced_speed`** (float) - High-resolution speed in m/s
- **`grade`** (float) - Grade/slope as percentage
- **`vertical_speed`** (float) - Vertical speed in m/s
- **`gps_accuracy`** (integer) - GPS accuracy in meters

### Power & Performance
Cycling power and efficiency metrics:

- **`power`** (integer) - Instantaneous power in watts
- **`accumulated_power`** (integer) - Total accumulated power in watts
- **`motor_power`** (integer) - E-bike motor power in watts
- **`left_torque_effectiveness`** (float) - Left leg torque effectiveness percentage
- **`right_torque_effectiveness`** (float) - Right leg torque effectiveness percentage
- **`left_pedal_smoothness`** (float) - Left pedal smoothness percentage
- **`right_pedal_smoothness`** (float) - Right pedal smoothness percentage
- **`combined_pedal_smoothness`** (float) - Combined pedal smoothness percentage

### Cadence & Cycling
Pedaling and cycling-specific metrics:

- **`cadence`** (integer) - Cadence in revolutions per minute
- **`cadence256`** (float) - High-resolution cadence in RPM
- **`fractional_cadence`** (float) - Fractional component of cadence
- **`left_right_balance`** (integer) - Power balance between left/right legs
- **`cycle_length`** (float) - Length per stroke/pedal cycle
- **`cycle_length16`** (float) - 16-bit resolution cycle length
- **`cycles`** (integer) - Number of cycles in this record
- **`total_cycles`** (integer) - Cumulative total cycles

### Running Dynamics
Advanced running form metrics:

- **`vertical_oscillation`** (float) - Vertical oscillation in millimeters
- **`stance_time`** (float) - Ground contact time in milliseconds
- **`stance_time_percent`** (float) - Stance time as percentage of stride
- **`stance_time_balance`** (float) - Left/right ground contact balance
- **`step_length`** (float) - Step length in millimeters
- **`vertical_ratio`** (float) - Vertical oscillation to step length ratio

### Physiological Data
Body metrics and wellness data:

- **`calories`** (integer) - Calories burned
- **`temperature`** (integer) - Ambient temperature in Celsius
- **`core_temperature`** (float) - Core body temperature in Celsius
- **`respiration_rate`** (integer) - Breathing rate in breaths per minute
- **`enhanced_respiration_rate`** (float) - High-resolution respiration rate
- **`current_stress`** (float) - Current stress level

### Blood/Oxygen Data
Advanced physiological monitoring:

- **`total_hemoglobin_conc`** (float) - Total hemoglobin concentration
- **`total_hemoglobin_conc_min`** (float) - Minimum hemoglobin concentration
- **`total_hemoglobin_conc_max`** (float) - Maximum hemoglobin concentration
- **`saturated_hemoglobin_percent`** (float) - Blood oxygen saturation (SpO2) percentage
- **`saturated_hemoglobin_percent_min`** (float) - Minimum SpO2 percentage
- **`saturated_hemoglobin_percent_max`** (float) - Maximum SpO2 percentage

### E-bike Specific
Electric bicycle telemetry:

- **`battery_soc`** (float) - Battery state of charge percentage
- **`ebike_travel_range`** (integer) - Estimated remaining range
- **`ebike_battery_level`** (integer) - Battery level percentage
- **`ebike_assist_mode`** (integer) - Current assist mode setting
- **`ebike_assist_level_percent`** (integer) - Assist level percentage

### Swimming/Water Sports
Aquatic activity metrics:

- **`stroke_type`** (integer) - Swimming stroke type (encoded value)
- **`resistance`** (integer) - Water resistance level
- **`ball_speed`** (float) - Ball speed for sports applications

### Diving
Scuba diving and underwater metrics:

- **`depth`** (float) - Water depth in meters
- **`absolute_pressure`** (integer) - Absolute pressure
- **`next_stop_depth`** (float) - Next decompression stop depth
- **`next_stop_time`** (integer) - Time to next decompression stop
- **`time_to_surface`** (integer) - Estimated time to surface
- **`ndl_time`** (integer) - No decompression limit time remaining
- **`cns_load`** (integer) - CNS oxygen toxicity load percentage
- **`n2_load`** (integer) - Nitrogen load for decompression
- **`air_time_remaining`** (integer) - Remaining air time
- **`ascent_rate`** (float) - Current ascent rate
- **`po2`** (float) - Partial pressure of oxygen
- **`pressure_sac`** (float) - Surface air consumption rate (pressure)
- **`volume_sac`** (float) - Surface air consumption rate (volume)
- **`rmv`** (float) - Respiratory minute volume

### Other Fields
Miscellaneous activity data:

- **`activity_type`** (integer) - Type of activity being recorded
- **`device_index`** (integer) - Index of recording device
- **`zone`** (integer) - Current training zone
- **`time128`** (float) - High-resolution timestamp (128Hz)
- **`grit`** (float) - Grit score (difficulty/intensity metric)
- **`flow`** (float) - Flow score (performance smoothness)
- **`time_from_course`** (float) - Time deviation from planned course
- **`left_pco`** (integer) - Left platform center offset
- **`right_pco`** (integer) - Right platform center offset

## Usage Examples

### Basic Usage
```elixir
# Read and decode FIT file
{:ok, fit_data} = File.read("activity.fit")
records = FitDecoder.decode(fit_data)

# Access basic fields
Enum.each(records, fn record ->
  IO.puts("Time: #{record.timestamp}")
  IO.puts("Distance: #{record.distance}m")
  if Map.has_key?(record, :heart_rate) do
    IO.puts("HR: #{record.heart_rate} bpm")
  end
end)
```

### Power Analysis
```elixir
# Extract power data for cycling analysis
power_data = records
|> Enum.filter(&Map.has_key?(&1, :power))
|> Enum.map(fn record ->
  %{
    timestamp: record.timestamp,
    power: record.power,
    cadence: Map.get(record, :cadence),
    heart_rate: Map.get(record, :heart_rate)
  }
end)

avg_power = power_data 
|> Enum.map(& &1.power) 
|> Enum.sum() 
|> div(length(power_data))

IO.puts("Average power: #{avg_power}W")
```

### GPS Tracking
```elixir
# Extract GPS coordinates
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
```

### Running Dynamics
```elixir
# Analyze running form
running_metrics = records
|> Enum.filter(&Map.has_key?(&1, :vertical_oscillation))
|> Enum.map(fn record ->
  %{
    vertical_oscillation: record.vertical_oscillation,
    stance_time: Map.get(record, :stance_time),
    step_length: Map.get(record, :step_length),
    vertical_ratio: Map.get(record, :vertical_ratio)
  }
end)
```

### Advanced Physiological Data
```elixir
# Extract comprehensive health metrics
health_data = records
|> Enum.map(fn record ->
  %{
    timestamp: record.timestamp,
    heart_rate: Map.get(record, :heart_rate),
    respiration_rate: Map.get(record, :respiration_rate),
    spo2: Map.get(record, :saturated_hemoglobin_percent),
    stress: Map.get(record, :current_stress),
    core_temp: Map.get(record, :core_temperature)
  }
  |> Enum.filter(fn {_k, v} -> v != nil end)
  |> Map.new()
end)
```

## Field Availability

Not all fields will be present in every FIT file. Field availability depends on:

1. **Device capabilities** - Only devices with specific sensors can record certain metrics
2. **Activity type** - Swimming fields won't appear in cycling activities
3. **Recording settings** - Some fields may be disabled to save battery/storage
4. **FIT file version** - Newer fields may not exist in older files

Always check for field presence using `Map.has_key?/2` before accessing values.

## Data Types

- **integer** - Whole numbers (timestamps, counts, enums)
- **float** - Decimal numbers (distances, speeds, percentages)

Invalid/missing values are filtered out during decoding, so fields are only present when they contain valid data.

## Converting Common Fields

### GPS Coordinates
```elixir
lat_degrees = record.position_lat / :math.pow(2, 31) * 180
lng_degrees = record.position_long / :math.pow(2, 31) * 180
```

### Timestamps
```elixir
# FIT timestamps are seconds since 1989-12-31 00:00:00 UTC
fit_epoch = ~U[1989-12-31 00:00:00Z] |> DateTime.to_unix()
actual_time = DateTime.from_unix!(fit_epoch + record.timestamp)
```

### Speed Conversions
```elixir
# m/s to km/h
speed_kmh = record.speed * 3.6

# m/s to mph  
speed_mph = record.speed * 2.237
```

This comprehensive field support enables detailed analysis of fitness activities, from basic tracking to advanced performance metrics and specialized applications like diving and e-bike monitoring.