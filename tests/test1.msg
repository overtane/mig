/* 
 * MIG test messages
 */

type int8   = 1;
type int16  = 2;
type int32  = 4;
type int64  = 8;
type uint8  = 1;
type uint16 = 2;
type uint32 = 4;
type uint64 = 8;
type bool   = 1;
type string = var;
type blob   = var;
type void   = 0;

// enum with two values
enum TestEnum1 {
  VALUE1 = 0;
  VALUE2 = 1;
}

// group with one parameter
group TestGroup1 {
  void Param1 = 0;
}

// message without parameters
message TestMessage1001 = 0x1001 { }

// message with one mandatory parameter
message TestMessage1002 = 0x1002 {
  void Param1 = 0; 
}

// message with several mandatory parameters
message TestMessage1003 = 0x1003 {
  void Param1 = 0; 
  bool Param2 = 1; 
  uint8 Param3 = 2; 
  blob Param4 = 3; 
}

// message with several optional parameters
message TestMessage1004 = 0x1004 {
  void Param1 = 0 [optional]; 
  bool Param2 = 1 [ optional ]; 
  // syntax allows repeated attributes (just testing parser)
  uint8 Param3 = 2 [ optional,optional, repeated, optional, repeated, repeated ]; 
  blob Param4 = 3 ; 
}

