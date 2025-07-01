# FIT Decoder Test Suite

This directory contains comprehensive tests for the FIT decoder NIF library.

## Test Structure

### Main Test File
- `fit_decoder_test.exs` - Main test suite with comprehensive coverage

### Support Files
- `test_helper.exs` - Test configuration and setup
- `support/test_data.ex` - Test utilities and helper functions

## Test Categories

### 1. Basic Input Validation (`decode_fit_file/1 with basic inputs`)
- Empty binary handling
- Invalid binary data
- Input type validation (string, nil, integer)

### 2. Real FIT Data Processing (`decode_fit_file/1 with real FIT data`)
- Valid FIT file decoding (when test file is available)
- Record structure validation
- Field availability handling

### 3. Data Integrity Validation (`data integrity validation`)
- Timestamp validation and bounds checking
- Distance progression validation
- Heart rate physiological bounds

### 4. NIF Functionality (`NIF functionality`)
- NIF module loading verification
- Function export validation
- Module delegation testing

### 5. Error Handling (`error handling`)
- SDK exception handling
- Large invalid binary handling
- Graceful error recovery

## Test Data

The tests are designed to work with or without external test data:

- **With test file**: If `/Users/nfishel/Downloads/test.fit` exists, comprehensive tests run with real data
- **Without test file**: Basic functionality tests run, file-dependent tests are skipped with informational messages

## Running Tests

```bash
# Run all tests
mix test

# Run with detailed output
mix test --trace

# Run only doctests
mix test --only doctest

# Run specific test file
mix test test/fit_decoder_test.exs
```

## Test Utilities

The `FitDecoderTest.TestData` module provides:

- `test_fit_file_path/0` - Returns path to test file if available
- `read_test_fit_file/0` - Safely reads test file with error handling
- `invalid_fit_binary/0` - Returns invalid binary for error testing
- `valid_record?/1` - Validates record structure and data types
- `extract_stats/1` - Extracts statistics from decoded records

## Expected Test Output

When running with a valid test file, you should see output like:

```
Decoded 387 records from FIT file
Distance range: 0.0 - 1985.8199462890625
Heart rate range: 95 - 191
Distance progression: 0.0 -> 1985.8199462890625
Average heart rate: 144.8 bpm

4 doctests, 15 tests, 0 failures
```

## Test Coverage

The test suite covers:

- ✅ NIF loading and initialization
- ✅ Input validation and error handling
- ✅ FIT file integrity checking
- ✅ Record data structure validation
- ✅ Data type correctness
- ✅ Physiological bounds checking
- ✅ Memory safety with large inputs
- ✅ Exception handling
- ✅ Documentation examples (doctests)

## Adding New Tests

When adding new tests:

1. Use the existing test structure and helpers
2. Handle cases where test data may not be available
3. Add appropriate assertions for data validation
4. Include both positive and negative test cases
5. Update this README if adding new test categories