class EmpireAI extends AIController 
{
  function Start();
}

function EmpireAI::Start()
{
  AILog.Info("EmpireAI starting");

  while (true) {             
    this.Sleep(1);
  }
}