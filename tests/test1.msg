/* 
 * MIG test messages
 */

typename int8   = 1;
typename int16  = 2;
typename int32  = 4;
typename int64  = 8;
typename uint8  = 1;
typename uint16 = 2;
typename uint32 = 4;
typename uint64 = 8;
typename bool   = 1;
typename string = var;
typename blob   = var;
typename void   = 0;

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

