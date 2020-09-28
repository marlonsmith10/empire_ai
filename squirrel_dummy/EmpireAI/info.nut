class EmpireAI extends AIInfo {
  function GetAuthor()      { return "Marlon Smith"; }
  function GetName()        { return "EmpireAI"; }
  function GetDescription() { return "A dummy squirrel AI, the real one is written in C++"; }
  function GetVersion()     { return 1; }
  function GetDate()        { return "2020-08-13"; }
  function CreateInstance() { return "EmpireAI"; }
  function GetShortName()   { return "XXXX"; }
  function GetAPIVersion()  { return "1.9"; }
}

/* Tell the core we are an AI */
RegisterAI(EmpireAI());