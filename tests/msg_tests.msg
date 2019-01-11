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
type string = std::string [composite];
type blob   = ::mig::blob_t [composite];
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
  void Param1 = 0;
}

// message with scalar parameters
message TestMessage1002 = 4098 {
  void param1 = 0;
  uint8 param2 = 1;
  int16 param3 = 2;
  uint32 param4 = 3 [optional];
}

// message with several mandatory parameters
message TestMessage1003 = 4099 {
  void Param1 = 0; 
  bool Param2 = 1; 
  uint8 Param3 = 2; 
  blob Param4 = 3; 
}

// message with several optional parameters
message TestMessage1004 = 5000 {
  void Param1 = 0 [optional]; 
  bool Param2 = 1 [ optional ]; 
  // syntax allows repeated attributes (just testing parser)
  uint8 Param3 = 2 [ optional,optional, repeated, optional, repeated, repeated ]; 
  blob Param4 = 3 ; 
}

// message with aggregate parameters
message TestMessage1005 = 5001 {
  TestEnum1 Param1 = 92;
  TestGroup1 Param2 = 12 [optional];
}

