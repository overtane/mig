/* 
 * MIG test messages
 */

// Top level elements
// types: elementary data types
// enums: value enumerations
// group: 

// elementary data types
// these can be used as types of parameters
type int8   = int8_t;
type int16  = int16_t;
type int32  = int32_t;
type int64  = int64_t;
type uint8  = uint8_t;
type uint16 = uint16_t;
type uint32 = uint32_t;
type uint64 = uint64_t;
type bool   = bool;
type string = std::string [var];
type blob   = ::mig::blob_t [var];
type void   = ::mig::void_t;

// enum with two values
enum TestEnum1 {
  VALUE1 = 0;
  VALUE2 = 1;
}

// message without parameters
message TestMessage1001 = 4097 { }

// group with one parameter
group TestGroup1 {
  void param1 = 0;
  uint32 param2 = 9;
}

// message with scalar parameters
message TestMessage1002 = 4098 {
  void param1 = 0;
  uint8 param2 = 1;
  int16 param3 = 2;
  uint32 param4 = 3 [optional];
  TestEnum1 param5 = 12 [optional];
  bool param6 = 13 [optional];
}

// message with composite parameters
message TestMessage1003 = 4099 {
  string param1 = 2; 
  blob param2 = 4; 
  TestGroup1 param3 = 6; 
  void param4 = 3 [optional, optional];
  uint8 param5 = 5; 
}

