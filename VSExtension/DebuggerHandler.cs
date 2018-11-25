using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Shell;
using EnvDTE80;
using EnvDTE;

namespace VSExtension
{
    public class DebuggerHandler
    {
        private DebuggerEvents debuggerEvents;

        public DebuggerHandler(DTE2 dte)
        {
            ThreadHelper.ThrowIfNotOnUIThread("Exception in DebuggerHandler()");

            debuggerEvents = dte.Events.DebuggerEvents;

            debuggerEvents.OnEnterBreakMode +=
                new _dispDebuggerEvents_OnEnterBreakModeEventHandler(this.BreakHandler);

            debuggerEvents.OnExceptionThrown +=
                new _dispDebuggerEvents_OnExceptionThrownEventHandler(this.OnExceptionThrownHandler);

            debuggerEvents.OnExceptionNotHandled +=
                new _dispDebuggerEvents_OnExceptionNotHandledEventHandler(this.OnExceptionNotHandled);

            debuggerEvents.OnEnterRunMode +=
                new _dispDebuggerEvents_OnEnterRunModeEventHandler(this.OnEnterRunModeHandler);
        }

        public void BreakHandler(dbgEventReason reason, ref dbgExecutionAction execAction)
        {
            System.Windows.Forms.MessageBox.Show("Debugger enters break mode. " +
                                                 "Reason: " + reason.ToString());
        }

        public void OnEnterRunModeHandler(dbgEventReason reason)
        {
            System.Windows.Forms.MessageBox.Show("Debugger enters run mode. " +
                                                 "Reason: " + reason.ToString());
        }

        public void OnExceptionThrownHandler
            (
                string ExceptionType, 
                string Name, 
                int Code, 
                string Description, 
                ref dbgExceptionAction ExceptionAction
            )
        {
            System.Windows.Forms.MessageBox.Show
                ("Exteption thrown.\n" +
                 "Code: " + Code.ToString() +
                 "\nDescription: " + Description + 
                 "\nException Type: " + ExceptionType +
                 "\nException Action: " + ExceptionAction.ToString());
        }

        public void OnExceptionNotHandled
    (
        string ExceptionType,
        string Name,
        int Code,
        string Description,
        ref dbgExceptionAction ExceptionAction
    )
        {
            System.Windows.Forms.MessageBox.Show
                ("Exteption not handled.\n" +
                 "Code: " + Code.ToString() +
                 "\nDescription: " + Description +
                 "\nException Type: " + ExceptionType +
                 "\nException Action: " + ExceptionAction.ToString());
        }
    }
}
