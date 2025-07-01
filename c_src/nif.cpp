#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>

#include "erl_nif.h"
#include "fit_decode.hpp"
#include "fit_record_mesg.hpp"
#include "fit_mesg_listener.hpp"
#include "fit_profile.hpp"

// A struct to hold the data from a single Record message.
struct RecordData {
    // Basic fields
    unsigned int timestamp;
    float altitude;
    float distance;
    unsigned int heart_rate;

    // Position & Navigation
    int position_lat;
    int position_long;
    float enhanced_altitude;
    float speed;
    float enhanced_speed;
    float grade;
    float vertical_speed;
    unsigned int gps_accuracy;

    // Power & Performance
    unsigned int power;
    unsigned int accumulated_power;
    unsigned int motor_power;
    float left_torque_effectiveness;
    float right_torque_effectiveness;
    float left_pedal_smoothness;
    float right_pedal_smoothness;
    float combined_pedal_smoothness;

    // Cadence & Cycling
    unsigned int cadence;
    float cadence256;
    float fractional_cadence;
    unsigned int left_right_balance;
    float cycle_length;
    float cycle_length16;
    unsigned int cycles;
    unsigned int total_cycles;

    // Running Dynamics
    float vertical_oscillation;
    float stance_time;
    float stance_time_percent;
    float stance_time_balance;
    float step_length;
    float vertical_ratio;

    // Physiological Data
    unsigned int calories;
    int temperature;
    float core_temperature;
    unsigned int respiration_rate;
    float enhanced_respiration_rate;
    float current_stress;

    // Blood/Oxygen Data
    float total_hemoglobin_conc;
    float total_hemoglobin_conc_min;
    float total_hemoglobin_conc_max;
    float saturated_hemoglobin_percent;
    float saturated_hemoglobin_percent_min;
    float saturated_hemoglobin_percent_max;

    // E-bike Specific
    float battery_soc;
    unsigned int ebike_travel_range;
    unsigned int ebike_battery_level;
    unsigned int ebike_assist_mode;
    unsigned int ebike_assist_level_percent;

    // Swimming/Water Sports
    unsigned int stroke_type;
    unsigned int resistance;
    float ball_speed;

    // Diving
    float depth;
    unsigned int absolute_pressure;
    float next_stop_depth;
    unsigned int next_stop_time;
    unsigned int time_to_surface;
    unsigned int ndl_time;
    unsigned int cns_load;
    unsigned int n2_load;
    unsigned int air_time_remaining;
    float ascent_rate;
    float po2;

    // Other Fields
    unsigned int activity_type;
    unsigned int device_index;
    unsigned int zone;
    float time128;
    float grit;
    float flow;
    float time_from_course;
    int left_pco;
    int right_pco;
    float pressure_sac;
    float volume_sac;
    float rmv;
};

// The listener class that processes messages from the FIT file.
class Listener : public fit::MesgListener {
public:
    std::vector<RecordData> records;

    // This method is called for every message in the file.
    void OnMesg(fit::Mesg& mesg) override {
        // Check if this is a Record message (message number 20)
        if (mesg.GetNum() == FIT_MESG_NUM_RECORD) {
            fit::RecordMesg recordMesg(mesg);
            ProcessRecordMessage(recordMesg);
        }
    }

private:
    void ProcessRecordMessage(fit::RecordMesg& mesg) {
        RecordData data = {};

        // Basic fields
        if (mesg.IsTimestampValid()) {
            data.timestamp = mesg.GetTimestamp() + 631065600; // Convert to Unix timestamp
        } else {
            data.timestamp = FIT_DATE_TIME_INVALID;
        }

        if (mesg.IsAltitudeValid()) {
            data.altitude = mesg.GetAltitude();
        } else {
            data.altitude = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsDistanceValid()) {
            data.distance = mesg.GetDistance();
        } else {
            data.distance = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsHeartRateValid()) {
            data.heart_rate = mesg.GetHeartRate();
        } else {
            data.heart_rate = FIT_UINT8_INVALID;
        }

        // Position & Navigation
        if (mesg.IsPositionLatValid()) {
            data.position_lat = mesg.GetPositionLat();
        } else {
            data.position_lat = FIT_SINT32_INVALID;
        }

        if (mesg.IsPositionLongValid()) {
            data.position_long = mesg.GetPositionLong();
        } else {
            data.position_long = FIT_SINT32_INVALID;
        }

        if (mesg.IsEnhancedAltitudeValid()) {
            data.enhanced_altitude = mesg.GetEnhancedAltitude();
        } else {
            data.enhanced_altitude = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsSpeedValid()) {
            data.speed = mesg.GetSpeed();
        } else {
            data.speed = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsEnhancedSpeedValid()) {
            data.enhanced_speed = mesg.GetEnhancedSpeed();
        } else {
            data.enhanced_speed = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsGradeValid()) {
            data.grade = mesg.GetGrade();
        } else {
            data.grade = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsVerticalSpeedValid()) {
            data.vertical_speed = mesg.GetVerticalSpeed();
        } else {
            data.vertical_speed = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsGpsAccuracyValid()) {
            data.gps_accuracy = mesg.GetGpsAccuracy();
        } else {
            data.gps_accuracy = FIT_UINT8_INVALID;
        }

        // Power & Performance
        if (mesg.IsPowerValid()) {
            data.power = mesg.GetPower();
        } else {
            data.power = FIT_UINT16_INVALID;
        }

        if (mesg.IsAccumulatedPowerValid()) {
            data.accumulated_power = mesg.GetAccumulatedPower();
        } else {
            data.accumulated_power = FIT_UINT32_INVALID;
        }

        if (mesg.IsMotorPowerValid()) {
            data.motor_power = mesg.GetMotorPower();
        } else {
            data.motor_power = FIT_UINT16_INVALID;
        }

        if (mesg.IsLeftTorqueEffectivenessValid()) {
            data.left_torque_effectiveness = mesg.GetLeftTorqueEffectiveness();
        } else {
            data.left_torque_effectiveness = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsRightTorqueEffectivenessValid()) {
            data.right_torque_effectiveness = mesg.GetRightTorqueEffectiveness();
        } else {
            data.right_torque_effectiveness = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsLeftPedalSmoothnessValid()) {
            data.left_pedal_smoothness = mesg.GetLeftPedalSmoothness();
        } else {
            data.left_pedal_smoothness = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsRightPedalSmoothnessValid()) {
            data.right_pedal_smoothness = mesg.GetRightPedalSmoothness();
        } else {
            data.right_pedal_smoothness = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsCombinedPedalSmoothnessValid()) {
            data.combined_pedal_smoothness = mesg.GetCombinedPedalSmoothness();
        } else {
            data.combined_pedal_smoothness = FIT_FLOAT32_INVALID;
        }

        // Cadence & Cycling
        if (mesg.IsCadenceValid()) {
            data.cadence = mesg.GetCadence();
        } else {
            data.cadence = FIT_UINT8_INVALID;
        }

        if (mesg.IsCadence256Valid()) {
            data.cadence256 = mesg.GetCadence256();
        } else {
            data.cadence256 = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsFractionalCadenceValid()) {
            data.fractional_cadence = mesg.GetFractionalCadence();
        } else {
            data.fractional_cadence = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsLeftRightBalanceValid()) {
            data.left_right_balance = mesg.GetLeftRightBalance();
        } else {
            data.left_right_balance = FIT_UINT8_INVALID;
        }

        if (mesg.IsCycleLengthValid()) {
            data.cycle_length = mesg.GetCycleLength();
        } else {
            data.cycle_length = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsCycleLength16Valid()) {
            data.cycle_length16 = mesg.GetCycleLength16();
        } else {
            data.cycle_length16 = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsCyclesValid()) {
            data.cycles = mesg.GetCycles();
        } else {
            data.cycles = FIT_UINT8_INVALID;
        }

        if (mesg.IsTotalCyclesValid()) {
            data.total_cycles = mesg.GetTotalCycles();
        } else {
            data.total_cycles = FIT_UINT32_INVALID;
        }

        // Running Dynamics
        if (mesg.IsVerticalOscillationValid()) {
            data.vertical_oscillation = mesg.GetVerticalOscillation();
        } else {
            data.vertical_oscillation = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsStanceTimeValid()) {
            data.stance_time = mesg.GetStanceTime();
        } else {
            data.stance_time = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsStanceTimePercentValid()) {
            data.stance_time_percent = mesg.GetStanceTimePercent();
        } else {
            data.stance_time_percent = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsStanceTimeBalanceValid()) {
            data.stance_time_balance = mesg.GetStanceTimeBalance();
        } else {
            data.stance_time_balance = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsStepLengthValid()) {
            data.step_length = mesg.GetStepLength();
        } else {
            data.step_length = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsVerticalRatioValid()) {
            data.vertical_ratio = mesg.GetVerticalRatio();
        } else {
            data.vertical_ratio = FIT_FLOAT32_INVALID;
        }

        // Physiological Data
        if (mesg.IsCaloriesValid()) {
            data.calories = mesg.GetCalories();
        } else {
            data.calories = FIT_UINT16_INVALID;
        }

        if (mesg.IsTemperatureValid()) {
            data.temperature = mesg.GetTemperature();
        } else {
            data.temperature = FIT_SINT8_INVALID;
        }

        if (mesg.IsCoreTemperatureValid()) {
            data.core_temperature = mesg.GetCoreTemperature();
        } else {
            data.core_temperature = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsRespirationRateValid()) {
            data.respiration_rate = mesg.GetRespirationRate();
        } else {
            data.respiration_rate = FIT_UINT8_INVALID;
        }

        if (mesg.IsEnhancedRespirationRateValid()) {
            data.enhanced_respiration_rate = mesg.GetEnhancedRespirationRate();
        } else {
            data.enhanced_respiration_rate = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsCurrentStressValid()) {
            data.current_stress = mesg.GetCurrentStress();
        } else {
            data.current_stress = FIT_FLOAT32_INVALID;
        }

        // Blood/Oxygen Data
        if (mesg.IsTotalHemoglobinConcValid()) {
            data.total_hemoglobin_conc = mesg.GetTotalHemoglobinConc();
        } else {
            data.total_hemoglobin_conc = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsTotalHemoglobinConcMinValid()) {
            data.total_hemoglobin_conc_min = mesg.GetTotalHemoglobinConcMin();
        } else {
            data.total_hemoglobin_conc_min = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsTotalHemoglobinConcMaxValid()) {
            data.total_hemoglobin_conc_max = mesg.GetTotalHemoglobinConcMax();
        } else {
            data.total_hemoglobin_conc_max = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsSaturatedHemoglobinPercentValid()) {
            data.saturated_hemoglobin_percent = mesg.GetSaturatedHemoglobinPercent();
        } else {
            data.saturated_hemoglobin_percent = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsSaturatedHemoglobinPercentMinValid()) {
            data.saturated_hemoglobin_percent_min = mesg.GetSaturatedHemoglobinPercentMin();
        } else {
            data.saturated_hemoglobin_percent_min = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsSaturatedHemoglobinPercentMaxValid()) {
            data.saturated_hemoglobin_percent_max = mesg.GetSaturatedHemoglobinPercentMax();
        } else {
            data.saturated_hemoglobin_percent_max = FIT_FLOAT32_INVALID;
        }

        // E-bike Specific
        if (mesg.IsBatterySocValid()) {
            data.battery_soc = mesg.GetBatterySoc();
        } else {
            data.battery_soc = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsEbikeTravelRangeValid()) {
            data.ebike_travel_range = mesg.GetEbikeTravelRange();
        } else {
            data.ebike_travel_range = FIT_UINT16_INVALID;
        }

        if (mesg.IsEbikeBatteryLevelValid()) {
            data.ebike_battery_level = mesg.GetEbikeBatteryLevel();
        } else {
            data.ebike_battery_level = FIT_UINT8_INVALID;
        }

        if (mesg.IsEbikeAssistModeValid()) {
            data.ebike_assist_mode = mesg.GetEbikeAssistMode();
        } else {
            data.ebike_assist_mode = FIT_UINT8_INVALID;
        }

        if (mesg.IsEbikeAssistLevelPercentValid()) {
            data.ebike_assist_level_percent = mesg.GetEbikeAssistLevelPercent();
        } else {
            data.ebike_assist_level_percent = FIT_UINT8_INVALID;
        }

        // Swimming/Water Sports
        if (mesg.IsStrokeTypeValid()) {
            data.stroke_type = mesg.GetStrokeType();
        } else {
            data.stroke_type = FIT_UINT8_INVALID;
        }

        if (mesg.IsResistanceValid()) {
            data.resistance = mesg.GetResistance();
        } else {
            data.resistance = FIT_UINT8_INVALID;
        }

        if (mesg.IsBallSpeedValid()) {
            data.ball_speed = mesg.GetBallSpeed();
        } else {
            data.ball_speed = FIT_FLOAT32_INVALID;
        }

        // Diving
        if (mesg.IsDepthValid()) {
            data.depth = mesg.GetDepth();
        } else {
            data.depth = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsAbsolutePressureValid()) {
            data.absolute_pressure = mesg.GetAbsolutePressure();
        } else {
            data.absolute_pressure = FIT_UINT32_INVALID;
        }

        if (mesg.IsNextStopDepthValid()) {
            data.next_stop_depth = mesg.GetNextStopDepth();
        } else {
            data.next_stop_depth = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsNextStopTimeValid()) {
            data.next_stop_time = mesg.GetNextStopTime();
        } else {
            data.next_stop_time = FIT_UINT32_INVALID;
        }

        if (mesg.IsTimeToSurfaceValid()) {
            data.time_to_surface = mesg.GetTimeToSurface();
        } else {
            data.time_to_surface = FIT_UINT32_INVALID;
        }

        if (mesg.IsNdlTimeValid()) {
            data.ndl_time = mesg.GetNdlTime();
        } else {
            data.ndl_time = FIT_UINT32_INVALID;
        }

        if (mesg.IsCnsLoadValid()) {
            data.cns_load = mesg.GetCnsLoad();
        } else {
            data.cns_load = FIT_UINT8_INVALID;
        }

        if (mesg.IsN2LoadValid()) {
            data.n2_load = mesg.GetN2Load();
        } else {
            data.n2_load = FIT_UINT16_INVALID;
        }

        if (mesg.IsAirTimeRemainingValid()) {
            data.air_time_remaining = mesg.GetAirTimeRemaining();
        } else {
            data.air_time_remaining = FIT_UINT32_INVALID;
        }

        if (mesg.IsAscentRateValid()) {
            data.ascent_rate = mesg.GetAscentRate();
        } else {
            data.ascent_rate = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsPo2Valid()) {
            data.po2 = mesg.GetPo2();
        } else {
            data.po2 = FIT_FLOAT32_INVALID;
        }

        // Other Fields
        if (mesg.IsActivityTypeValid()) {
            data.activity_type = mesg.GetActivityType();
        } else {
            data.activity_type = FIT_UINT8_INVALID;
        }

        if (mesg.IsDeviceIndexValid()) {
            data.device_index = mesg.GetDeviceIndex();
        } else {
            data.device_index = FIT_UINT8_INVALID;
        }

        if (mesg.IsZoneValid()) {
            data.zone = mesg.GetZone();
        } else {
            data.zone = FIT_UINT8_INVALID;
        }

        if (mesg.IsTime128Valid()) {
            data.time128 = mesg.GetTime128();
        } else {
            data.time128 = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsGritValid()) {
            data.grit = mesg.GetGrit();
        } else {
            data.grit = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsFlowValid()) {
            data.flow = mesg.GetFlow();
        } else {
            data.flow = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsTimeFromCourseValid()) {
            data.time_from_course = mesg.GetTimeFromCourse();
        } else {
            data.time_from_course = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsLeftPcoValid()) {
            data.left_pco = mesg.GetLeftPco();
        } else {
            data.left_pco = FIT_SINT8_INVALID;
        }

        if (mesg.IsRightPcoValid()) {
            data.right_pco = mesg.GetRightPco();
        } else {
            data.right_pco = FIT_SINT8_INVALID;
        }

        if (mesg.IsPressureSacValid()) {
            data.pressure_sac = mesg.GetPressureSac();
        } else {
            data.pressure_sac = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsVolumeSacValid()) {
            data.volume_sac = mesg.GetVolumeSac();
        } else {
            data.volume_sac = FIT_FLOAT32_INVALID;
        }

        if (mesg.IsRmvValid()) {
            data.rmv = mesg.GetRmv();
        } else {
            data.rmv = FIT_FLOAT32_INVALID;
        }

        // Only add record if we have valid timestamp and at least one other valid field
        if (data.timestamp != FIT_DATE_TIME_INVALID) {
            records.push_back(data);
        }
    }
};

// This is the main NIF function that Elixir will call.
static ERL_NIF_TERM decode_fit_file_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    if (argc != 1) {
        return enif_make_badarg(env);
    }

    ErlNifBinary fit_binary;
    if (!enif_inspect_binary(env, argv[0], &fit_binary)) {
        return enif_make_badarg(env);
    }

    // Create a string stream from the binary data from Elixir.
    std::stringstream fit_stream;
    fit_stream.write(reinterpret_cast<const char*>(fit_binary.data), fit_binary.size);

    fit::Decode decode;
    Listener listener;

    // Check if the FIT file is valid.
    if (!decode.CheckIntegrity(fit_stream)) {
        return enif_make_atom(env, "error_integrity_check_failed");
    }

    // Reset stream position after integrity check
    fit_stream.clear();
    fit_stream.seekg(0, std::ios::beg);

    // Read the file from the stream with our listener.
    try {
        decode.Read(fit_stream, listener);
    } catch (const fit::RuntimeException& e) {
        return enif_make_atom(env, "error_sdk_exception");
    }

    // --- Convert C++ data to Elixir terms ---

    // Create atoms for all the map keys
    ERL_NIF_TERM atom_timestamp = enif_make_atom(env, "timestamp");
    ERL_NIF_TERM atom_altitude = enif_make_atom(env, "altitude");
    ERL_NIF_TERM atom_distance = enif_make_atom(env, "distance");
    ERL_NIF_TERM atom_heart_rate = enif_make_atom(env, "heart_rate");

    // Position & Navigation atoms
    ERL_NIF_TERM atom_position_lat = enif_make_atom(env, "position_lat");
    ERL_NIF_TERM atom_position_long = enif_make_atom(env, "position_long");
    ERL_NIF_TERM atom_enhanced_altitude = enif_make_atom(env, "enhanced_altitude");
    ERL_NIF_TERM atom_speed = enif_make_atom(env, "speed");
    ERL_NIF_TERM atom_enhanced_speed = enif_make_atom(env, "enhanced_speed");
    ERL_NIF_TERM atom_grade = enif_make_atom(env, "grade");
    ERL_NIF_TERM atom_vertical_speed = enif_make_atom(env, "vertical_speed");
    ERL_NIF_TERM atom_gps_accuracy = enif_make_atom(env, "gps_accuracy");

    // Power & Performance atoms
    ERL_NIF_TERM atom_power = enif_make_atom(env, "power");
    ERL_NIF_TERM atom_accumulated_power = enif_make_atom(env, "accumulated_power");
    ERL_NIF_TERM atom_motor_power = enif_make_atom(env, "motor_power");
    ERL_NIF_TERM atom_left_torque_effectiveness = enif_make_atom(env, "left_torque_effectiveness");
    ERL_NIF_TERM atom_right_torque_effectiveness = enif_make_atom(env, "right_torque_effectiveness");
    ERL_NIF_TERM atom_left_pedal_smoothness = enif_make_atom(env, "left_pedal_smoothness");
    ERL_NIF_TERM atom_right_pedal_smoothness = enif_make_atom(env, "right_pedal_smoothness");
    ERL_NIF_TERM atom_combined_pedal_smoothness = enif_make_atom(env, "combined_pedal_smoothness");

    // Cadence & Cycling atoms
    ERL_NIF_TERM atom_cadence = enif_make_atom(env, "cadence");
    ERL_NIF_TERM atom_cadence256 = enif_make_atom(env, "cadence256");
    ERL_NIF_TERM atom_fractional_cadence = enif_make_atom(env, "fractional_cadence");
    ERL_NIF_TERM atom_left_right_balance = enif_make_atom(env, "left_right_balance");
    ERL_NIF_TERM atom_cycle_length = enif_make_atom(env, "cycle_length");
    ERL_NIF_TERM atom_cycle_length16 = enif_make_atom(env, "cycle_length16");
    ERL_NIF_TERM atom_cycles = enif_make_atom(env, "cycles");
    ERL_NIF_TERM atom_total_cycles = enif_make_atom(env, "total_cycles");

    // Running Dynamics atoms
    ERL_NIF_TERM atom_vertical_oscillation = enif_make_atom(env, "vertical_oscillation");
    ERL_NIF_TERM atom_stance_time = enif_make_atom(env, "stance_time");
    ERL_NIF_TERM atom_stance_time_percent = enif_make_atom(env, "stance_time_percent");
    ERL_NIF_TERM atom_stance_time_balance = enif_make_atom(env, "stance_time_balance");
    ERL_NIF_TERM atom_step_length = enif_make_atom(env, "step_length");
    ERL_NIF_TERM atom_vertical_ratio = enif_make_atom(env, "vertical_ratio");

    // Physiological Data atoms
    ERL_NIF_TERM atom_calories = enif_make_atom(env, "calories");
    ERL_NIF_TERM atom_temperature = enif_make_atom(env, "temperature");
    ERL_NIF_TERM atom_core_temperature = enif_make_atom(env, "core_temperature");
    ERL_NIF_TERM atom_respiration_rate = enif_make_atom(env, "respiration_rate");
    ERL_NIF_TERM atom_enhanced_respiration_rate = enif_make_atom(env, "enhanced_respiration_rate");
    ERL_NIF_TERM atom_current_stress = enif_make_atom(env, "current_stress");

    // Blood/Oxygen Data atoms
    ERL_NIF_TERM atom_total_hemoglobin_conc = enif_make_atom(env, "total_hemoglobin_conc");
    ERL_NIF_TERM atom_total_hemoglobin_conc_min = enif_make_atom(env, "total_hemoglobin_conc_min");
    ERL_NIF_TERM atom_total_hemoglobin_conc_max = enif_make_atom(env, "total_hemoglobin_conc_max");
    ERL_NIF_TERM atom_saturated_hemoglobin_percent = enif_make_atom(env, "saturated_hemoglobin_percent");
    ERL_NIF_TERM atom_saturated_hemoglobin_percent_min = enif_make_atom(env, "saturated_hemoglobin_percent_min");
    ERL_NIF_TERM atom_saturated_hemoglobin_percent_max = enif_make_atom(env, "saturated_hemoglobin_percent_max");

    // E-bike Specific atoms
    ERL_NIF_TERM atom_battery_soc = enif_make_atom(env, "battery_soc");
    ERL_NIF_TERM atom_ebike_travel_range = enif_make_atom(env, "ebike_travel_range");
    ERL_NIF_TERM atom_ebike_battery_level = enif_make_atom(env, "ebike_battery_level");
    ERL_NIF_TERM atom_ebike_assist_mode = enif_make_atom(env, "ebike_assist_mode");
    ERL_NIF_TERM atom_ebike_assist_level_percent = enif_make_atom(env, "ebike_assist_level_percent");

    // Swimming/Water Sports atoms
    ERL_NIF_TERM atom_stroke_type = enif_make_atom(env, "stroke_type");
    ERL_NIF_TERM atom_resistance = enif_make_atom(env, "resistance");
    ERL_NIF_TERM atom_ball_speed = enif_make_atom(env, "ball_speed");

    // Diving atoms
    ERL_NIF_TERM atom_depth = enif_make_atom(env, "depth");
    ERL_NIF_TERM atom_absolute_pressure = enif_make_atom(env, "absolute_pressure");
    ERL_NIF_TERM atom_next_stop_depth = enif_make_atom(env, "next_stop_depth");
    ERL_NIF_TERM atom_next_stop_time = enif_make_atom(env, "next_stop_time");
    ERL_NIF_TERM atom_time_to_surface = enif_make_atom(env, "time_to_surface");
    ERL_NIF_TERM atom_ndl_time = enif_make_atom(env, "ndl_time");
    ERL_NIF_TERM atom_cns_load = enif_make_atom(env, "cns_load");
    ERL_NIF_TERM atom_n2_load = enif_make_atom(env, "n2_load");
    ERL_NIF_TERM atom_air_time_remaining = enif_make_atom(env, "air_time_remaining");
    ERL_NIF_TERM atom_ascent_rate = enif_make_atom(env, "ascent_rate");
    ERL_NIF_TERM atom_po2 = enif_make_atom(env, "po2");

    // Other Fields atoms
    ERL_NIF_TERM atom_activity_type = enif_make_atom(env, "activity_type");
    ERL_NIF_TERM atom_device_index = enif_make_atom(env, "device_index");
    ERL_NIF_TERM atom_zone = enif_make_atom(env, "zone");
    ERL_NIF_TERM atom_time128 = enif_make_atom(env, "time128");
    ERL_NIF_TERM atom_grit = enif_make_atom(env, "grit");
    ERL_NIF_TERM atom_flow = enif_make_atom(env, "flow");
    ERL_NIF_TERM atom_time_from_course = enif_make_atom(env, "time_from_course");
    ERL_NIF_TERM atom_left_pco = enif_make_atom(env, "left_pco");
    ERL_NIF_TERM atom_right_pco = enif_make_atom(env, "right_pco");
    ERL_NIF_TERM atom_pressure_sac = enif_make_atom(env, "pressure_sac");
    ERL_NIF_TERM atom_volume_sac = enif_make_atom(env, "volume_sac");
    ERL_NIF_TERM atom_rmv = enif_make_atom(env, "rmv");

    ERL_NIF_TERM result_list = enif_make_list(env, 0);

    // Iterate through the collected records in reverse to build the Elixir list correctly.
    for (auto it = listener.records.rbegin(); it != listener.records.rend(); ++it) {
        ERL_NIF_TERM map = enif_make_new_map(env);

        // Add timestamp (always present due to our filtering)
        enif_make_map_put(env, map, atom_timestamp, enif_make_uint(env, it->timestamp), &map);

        // Basic fields
        if (it->altitude != FIT_FLOAT32_INVALID && !std::isnan(it->altitude)) {
            enif_make_map_put(env, map, atom_altitude, enif_make_double(env, it->altitude), &map);
        }

        if (it->distance != FIT_FLOAT32_INVALID && !std::isnan(it->distance)) {
            enif_make_map_put(env, map, atom_distance, enif_make_double(env, it->distance), &map);
        }

        if (it->heart_rate != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_heart_rate, enif_make_int(env, it->heart_rate), &map);
        }

        // Position & Navigation
        if (it->position_lat != FIT_SINT32_INVALID) {
            enif_make_map_put(env, map, atom_position_lat, enif_make_int(env, it->position_lat), &map);
        }

        if (it->position_long != FIT_SINT32_INVALID) {
            enif_make_map_put(env, map, atom_position_long, enif_make_int(env, it->position_long), &map);
        }

        if (it->enhanced_altitude != FIT_FLOAT32_INVALID && !std::isnan(it->enhanced_altitude)) {
            enif_make_map_put(env, map, atom_enhanced_altitude, enif_make_double(env, it->enhanced_altitude), &map);
        }

        if (it->speed != FIT_FLOAT32_INVALID && !std::isnan(it->speed)) {
            enif_make_map_put(env, map, atom_speed, enif_make_double(env, it->speed), &map);
        }

        if (it->enhanced_speed != FIT_FLOAT32_INVALID && !std::isnan(it->enhanced_speed)) {
            enif_make_map_put(env, map, atom_enhanced_speed, enif_make_double(env, it->enhanced_speed), &map);
        }

        if (it->grade != FIT_FLOAT32_INVALID && !std::isnan(it->grade)) {
            enif_make_map_put(env, map, atom_grade, enif_make_double(env, it->grade), &map);
        }

        if (it->vertical_speed != FIT_FLOAT32_INVALID && !std::isnan(it->vertical_speed)) {
            enif_make_map_put(env, map, atom_vertical_speed, enif_make_double(env, it->vertical_speed), &map);
        }

        if (it->gps_accuracy != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_gps_accuracy, enif_make_int(env, it->gps_accuracy), &map);
        }

        // Power & Performance
        if (it->power != FIT_UINT16_INVALID) {
            enif_make_map_put(env, map, atom_power, enif_make_int(env, it->power), &map);
        }

        if (it->accumulated_power != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_accumulated_power, enif_make_uint(env, it->accumulated_power), &map);
        }

        if (it->motor_power != FIT_UINT16_INVALID) {
            enif_make_map_put(env, map, atom_motor_power, enif_make_int(env, it->motor_power), &map);
        }

        if (it->left_torque_effectiveness != FIT_FLOAT32_INVALID && !std::isnan(it->left_torque_effectiveness)) {
            enif_make_map_put(env, map, atom_left_torque_effectiveness, enif_make_double(env, it->left_torque_effectiveness), &map);
        }

        if (it->right_torque_effectiveness != FIT_FLOAT32_INVALID && !std::isnan(it->right_torque_effectiveness)) {
            enif_make_map_put(env, map, atom_right_torque_effectiveness, enif_make_double(env, it->right_torque_effectiveness), &map);
        }

        if (it->left_pedal_smoothness != FIT_FLOAT32_INVALID && !std::isnan(it->left_pedal_smoothness)) {
            enif_make_map_put(env, map, atom_left_pedal_smoothness, enif_make_double(env, it->left_pedal_smoothness), &map);
        }

        if (it->right_pedal_smoothness != FIT_FLOAT32_INVALID && !std::isnan(it->right_pedal_smoothness)) {
            enif_make_map_put(env, map, atom_right_pedal_smoothness, enif_make_double(env, it->right_pedal_smoothness), &map);
        }

        if (it->combined_pedal_smoothness != FIT_FLOAT32_INVALID && !std::isnan(it->combined_pedal_smoothness)) {
            enif_make_map_put(env, map, atom_combined_pedal_smoothness, enif_make_double(env, it->combined_pedal_smoothness), &map);
        }

        // Cadence & Cycling
        if (it->cadence != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_cadence, enif_make_int(env, it->cadence), &map);
        }

        if (it->cadence256 != FIT_FLOAT32_INVALID && !std::isnan(it->cadence256)) {
            enif_make_map_put(env, map, atom_cadence256, enif_make_double(env, it->cadence256), &map);
        }

        if (it->fractional_cadence != FIT_FLOAT32_INVALID && !std::isnan(it->fractional_cadence)) {
            enif_make_map_put(env, map, atom_fractional_cadence, enif_make_double(env, it->fractional_cadence), &map);
        }

        if (it->left_right_balance != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_left_right_balance, enif_make_int(env, it->left_right_balance), &map);
        }

        if (it->cycle_length != FIT_FLOAT32_INVALID && !std::isnan(it->cycle_length)) {
            enif_make_map_put(env, map, atom_cycle_length, enif_make_double(env, it->cycle_length), &map);
        }

        if (it->cycle_length16 != FIT_FLOAT32_INVALID && !std::isnan(it->cycle_length16)) {
            enif_make_map_put(env, map, atom_cycle_length16, enif_make_double(env, it->cycle_length16), &map);
        }

        if (it->cycles != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_cycles, enif_make_int(env, it->cycles), &map);
        }

        if (it->total_cycles != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_total_cycles, enif_make_uint(env, it->total_cycles), &map);
        }

        // Running Dynamics
        if (it->vertical_oscillation != FIT_FLOAT32_INVALID && !std::isnan(it->vertical_oscillation)) {
            enif_make_map_put(env, map, atom_vertical_oscillation, enif_make_double(env, it->vertical_oscillation), &map);
        }

        if (it->stance_time != FIT_FLOAT32_INVALID && !std::isnan(it->stance_time)) {
            enif_make_map_put(env, map, atom_stance_time, enif_make_double(env, it->stance_time), &map);
        }

        if (it->stance_time_percent != FIT_FLOAT32_INVALID && !std::isnan(it->stance_time_percent)) {
            enif_make_map_put(env, map, atom_stance_time_percent, enif_make_double(env, it->stance_time_percent), &map);
        }

        if (it->stance_time_balance != FIT_FLOAT32_INVALID && !std::isnan(it->stance_time_balance)) {
            enif_make_map_put(env, map, atom_stance_time_balance, enif_make_double(env, it->stance_time_balance), &map);
        }

        if (it->step_length != FIT_FLOAT32_INVALID && !std::isnan(it->step_length)) {
            enif_make_map_put(env, map, atom_step_length, enif_make_double(env, it->step_length), &map);
        }

        if (it->vertical_ratio != FIT_FLOAT32_INVALID && !std::isnan(it->vertical_ratio)) {
            enif_make_map_put(env, map, atom_vertical_ratio, enif_make_double(env, it->vertical_ratio), &map);
        }

        // Physiological Data
        if (it->calories != FIT_UINT16_INVALID) {
            enif_make_map_put(env, map, atom_calories, enif_make_int(env, it->calories), &map);
        }

        if (it->temperature != FIT_SINT8_INVALID) {
            enif_make_map_put(env, map, atom_temperature, enif_make_int(env, it->temperature), &map);
        }

        if (it->core_temperature != FIT_FLOAT32_INVALID && !std::isnan(it->core_temperature)) {
            enif_make_map_put(env, map, atom_core_temperature, enif_make_double(env, it->core_temperature), &map);
        }

        if (it->respiration_rate != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_respiration_rate, enif_make_int(env, it->respiration_rate), &map);
        }

        if (it->enhanced_respiration_rate != FIT_FLOAT32_INVALID && !std::isnan(it->enhanced_respiration_rate)) {
            enif_make_map_put(env, map, atom_enhanced_respiration_rate, enif_make_double(env, it->enhanced_respiration_rate), &map);
        }

        if (it->current_stress != FIT_FLOAT32_INVALID && !std::isnan(it->current_stress)) {
            enif_make_map_put(env, map, atom_current_stress, enif_make_double(env, it->current_stress), &map);
        }

        // Blood/Oxygen Data
        if (it->total_hemoglobin_conc != FIT_FLOAT32_INVALID && !std::isnan(it->total_hemoglobin_conc)) {
            enif_make_map_put(env, map, atom_total_hemoglobin_conc, enif_make_double(env, it->total_hemoglobin_conc), &map);
        }

        if (it->total_hemoglobin_conc_min != FIT_FLOAT32_INVALID && !std::isnan(it->total_hemoglobin_conc_min)) {
            enif_make_map_put(env, map, atom_total_hemoglobin_conc_min, enif_make_double(env, it->total_hemoglobin_conc_min), &map);
        }

        if (it->total_hemoglobin_conc_max != FIT_FLOAT32_INVALID && !std::isnan(it->total_hemoglobin_conc_max)) {
            enif_make_map_put(env, map, atom_total_hemoglobin_conc_max, enif_make_double(env, it->total_hemoglobin_conc_max), &map);
        }

        if (it->saturated_hemoglobin_percent != FIT_FLOAT32_INVALID && !std::isnan(it->saturated_hemoglobin_percent)) {
            enif_make_map_put(env, map, atom_saturated_hemoglobin_percent, enif_make_double(env, it->saturated_hemoglobin_percent), &map);
        }

        if (it->saturated_hemoglobin_percent_min != FIT_FLOAT32_INVALID && !std::isnan(it->saturated_hemoglobin_percent_min)) {
            enif_make_map_put(env, map, atom_saturated_hemoglobin_percent_min, enif_make_double(env, it->saturated_hemoglobin_percent_min), &map);
        }

        if (it->saturated_hemoglobin_percent_max != FIT_FLOAT32_INVALID && !std::isnan(it->saturated_hemoglobin_percent_max)) {
            enif_make_map_put(env, map, atom_saturated_hemoglobin_percent_max, enif_make_double(env, it->saturated_hemoglobin_percent_max), &map);
        }

        // E-bike Specific
        if (it->battery_soc != FIT_FLOAT32_INVALID && !std::isnan(it->battery_soc)) {
            enif_make_map_put(env, map, atom_battery_soc, enif_make_double(env, it->battery_soc), &map);
        }

        if (it->ebike_travel_range != FIT_UINT16_INVALID) {
            enif_make_map_put(env, map, atom_ebike_travel_range, enif_make_int(env, it->ebike_travel_range), &map);
        }

        if (it->ebike_battery_level != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_ebike_battery_level, enif_make_int(env, it->ebike_battery_level), &map);
        }

        if (it->ebike_assist_mode != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_ebike_assist_mode, enif_make_int(env, it->ebike_assist_mode), &map);
        }

        if (it->ebike_assist_level_percent != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_ebike_assist_level_percent, enif_make_int(env, it->ebike_assist_level_percent), &map);
        }

        // Swimming/Water Sports
        if (it->stroke_type != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_stroke_type, enif_make_int(env, it->stroke_type), &map);
        }

        if (it->resistance != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_resistance, enif_make_int(env, it->resistance), &map);
        }

        if (it->ball_speed != FIT_FLOAT32_INVALID && !std::isnan(it->ball_speed)) {
            enif_make_map_put(env, map, atom_ball_speed, enif_make_double(env, it->ball_speed), &map);
        }

        // Diving
        if (it->depth != FIT_FLOAT32_INVALID && !std::isnan(it->depth)) {
            enif_make_map_put(env, map, atom_depth, enif_make_double(env, it->depth), &map);
        }

        if (it->absolute_pressure != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_absolute_pressure, enif_make_uint(env, it->absolute_pressure), &map);
        }

        if (it->next_stop_depth != FIT_FLOAT32_INVALID && !std::isnan(it->next_stop_depth)) {
            enif_make_map_put(env, map, atom_next_stop_depth, enif_make_double(env, it->next_stop_depth), &map);
        }

        if (it->next_stop_time != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_next_stop_time, enif_make_uint(env, it->next_stop_time), &map);
        }

        if (it->time_to_surface != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_time_to_surface, enif_make_uint(env, it->time_to_surface), &map);
        }

        if (it->ndl_time != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_ndl_time, enif_make_uint(env, it->ndl_time), &map);
        }

        if (it->cns_load != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_cns_load, enif_make_int(env, it->cns_load), &map);
        }

        if (it->n2_load != FIT_UINT16_INVALID) {
            enif_make_map_put(env, map, atom_n2_load, enif_make_int(env, it->n2_load), &map);
        }

        if (it->air_time_remaining != FIT_UINT32_INVALID) {
            enif_make_map_put(env, map, atom_air_time_remaining, enif_make_uint(env, it->air_time_remaining), &map);
        }

        if (it->ascent_rate != FIT_FLOAT32_INVALID && !std::isnan(it->ascent_rate)) {
            enif_make_map_put(env, map, atom_ascent_rate, enif_make_double(env, it->ascent_rate), &map);
        }

        if (it->po2 != FIT_FLOAT32_INVALID && !std::isnan(it->po2)) {
            enif_make_map_put(env, map, atom_po2, enif_make_double(env, it->po2), &map);
        }

        // Other Fields
        if (it->activity_type != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_activity_type, enif_make_int(env, it->activity_type), &map);
        }

        if (it->device_index != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_device_index, enif_make_int(env, it->device_index), &map);
        }

        if (it->zone != FIT_UINT8_INVALID) {
            enif_make_map_put(env, map, atom_zone, enif_make_int(env, it->zone), &map);
        }

        if (it->time128 != FIT_FLOAT32_INVALID && !std::isnan(it->time128)) {
            enif_make_map_put(env, map, atom_time128, enif_make_double(env, it->time128), &map);
        }

        if (it->grit != FIT_FLOAT32_INVALID && !std::isnan(it->grit)) {
            enif_make_map_put(env, map, atom_grit, enif_make_double(env, it->grit), &map);
        }

        if (it->flow != FIT_FLOAT32_INVALID && !std::isnan(it->flow)) {
            enif_make_map_put(env, map, atom_flow, enif_make_double(env, it->flow), &map);
        }

        if (it->time_from_course != FIT_FLOAT32_INVALID && !std::isnan(it->time_from_course)) {
            enif_make_map_put(env, map, atom_time_from_course, enif_make_double(env, it->time_from_course), &map);
        }

        if (it->left_pco != FIT_SINT8_INVALID) {
            enif_make_map_put(env, map, atom_left_pco, enif_make_int(env, it->left_pco), &map);
        }

        if (it->right_pco != FIT_SINT8_INVALID) {
            enif_make_map_put(env, map, atom_right_pco, enif_make_int(env, it->right_pco), &map);
        }

        if (it->pressure_sac != FIT_FLOAT32_INVALID && !std::isnan(it->pressure_sac)) {
            enif_make_map_put(env, map, atom_pressure_sac, enif_make_double(env, it->pressure_sac), &map);
        }

        if (it->volume_sac != FIT_FLOAT32_INVALID && !std::isnan(it->volume_sac)) {
            enif_make_map_put(env, map, atom_volume_sac, enif_make_double(env, it->volume_sac), &map);
        }

        if (it->rmv != FIT_FLOAT32_INVALID && !std::isnan(it->rmv)) {
            enif_make_map_put(env, map, atom_rmv, enif_make_double(env, it->rmv), &map);
        }

        result_list = enif_make_list_cell(env, map, result_list);
    }

    return result_list;
}

// The list of functions this NIF exports.
static ErlNifFunc nif_funcs[] = {
    {"decode_fit_file", 1, decode_fit_file_nif}
};

// Initialize the NIF library.
ERL_NIF_INIT(Elixir.FitDecoder.NIF, nif_funcs, NULL, NULL, NULL, NULL)
