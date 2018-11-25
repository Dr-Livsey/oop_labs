using System;
using System.ComponentModel.Design;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Task = System.Threading.Tasks.Task;
using System.Collections.Generic;
using EnvDTE80;
using EnvDTE;

namespace VSExtension
{
    /// <summary>
    /// Command handler
    /// </summary>
    internal sealed class ExtToolWindowCommand
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        public const int CommandId = 0x0100;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("3f8e00c0-e65a-4335-b483-435f7753189d");

        /// <summary>
        /// VS Package that provides this command, not null.
        /// </summary>
        private readonly AsyncPackage package;

        DebuggerHandler dHandler;

        /// <summary>
        /// Initializes a new instance of the <see cref="ExtToolWindowCommand"/> class.
        /// Adds our command handlers for menu (commands must exist in the command table file)
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        /// <param name="commandService">Command service to add command to, not null.</param>
        private ExtToolWindowCommand(AsyncPackage package, OleMenuCommandService commandService)
        {
            this.package = package ?? throw new ArgumentNullException(nameof(package));
            commandService = commandService ?? throw new ArgumentNullException(nameof(commandService));

            ThreadHelper.ThrowIfNotOnUIThread("Exception in ExtToolWindowCommand()");

            codeInfoPane = dte.ToolWindows.OutputWindow.OutputWindowPanes.Add("Code Info VSPackage");
            debugPane = dte.ToolWindows.OutputWindow.OutputWindowPanes.Add("Debug VSPackage");

            dHandler = new DebuggerHandler(dte);

            var menuCommandID = new CommandID(CommandSet, CommandId);
            var menuItem = new MenuCommand(this.Execute, menuCommandID);
            commandService.AddCommand(menuItem);
        }

        /// <summary>
        /// Gets the instance of the command.
        /// </summary>
        public static ExtToolWindowCommand Instance
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the service provider from the owner package.
        /// </summary>
        private Microsoft.VisualStudio.Shell.IAsyncServiceProvider ServiceProvider
        {
            get
            {
                return this.package;
            }
        }

        private static DTE2 dte;
        private OutputWindowPane codeInfoPane;
        private OutputWindowPane debugPane; /*Only for debug*/

        /// <summary>
        /// Initializes the singleton instance of the command.
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        public static async Task InitializeAsync(AsyncPackage package)
        {
            // Switch to the main thread - the call to AddCommand in ExtToolWindowCommand's constructor requires
            // the UI thread.
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(package.DisposalToken);

            dte = await package.GetServiceAsync(typeof(DTE)) as DTE2 ?? throw new ArgumentNullException("DTE is null");

            OleMenuCommandService commandService = await package.GetServiceAsync((typeof(IMenuCommandService))) as OleMenuCommandService;
            Instance = new ExtToolWindowCommand(package, commandService);
        }

        /// <summary>
        /// Shows the tool window when the menu item is clicked.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        private void Execute(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread("Exception in Execute()");

            // Get the instance number 0 of this tool window. This window is single instance so this instance
            // is actually the only one.
            // The last flag is set to true so that if the tool window does not exists it will be created.
            ToolWindowPane window = this.package.FindToolWindow(typeof(ExtToolWindow), 0, true);
            if ((null == window) || (null == window.Frame))
            {
                throw new NotSupportedException("Cannot create tool window");
            }

            IVsWindowFrame windowFrame = (IVsWindowFrame)window.Frame;
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(windowFrame.Show());

            FileCodeModel2 codeModel = getCurrentCodeModel();

            ExtToolWindowControl toolWindow = window.Content as ExtToolWindowControl;

            toolWindow.InfoTable.ItemsSource = getTableList(codeModel);
        }

        private FileCodeModel2 getCurrentCodeModel()
        {
            //if (dte == null)
            //    throw new System.ArgumentNullException("DTE is null");

            ThreadHelper.ThrowIfNotOnUIThread("Exception in getCurrentCodeElement()");

            try
            {
                ProjectItem activeItem = dte.ActiveDocument is null ? null : dte.ActiveDocument.ProjectItem;

                codeInfoPane.OutputString("\n---------------------Code Table Info---------------------\n");

                if (activeItem != null && activeItem.FileCodeModel != null)
                {
                    codeInfoPane.OutputString("# Solution ---> " + dte.Solution.FileName + "\n");
                    codeInfoPane.OutputString("# Selected item ---> " + activeItem.Name + "");
                }
                else codeInfoPane.OutputString("# No active source code");

                codeInfoPane.OutputString("\n--------------------------------------------------------\n");

                return activeItem is null ? null : activeItem.FileCodeModel as FileCodeModel2;
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.ToString());
            }

            return null;
        }

        private List<TableRecord> getTableList(FileCodeModel2 activeCodeModel)
        {
            if (activeCodeModel == null)
            {
                System.Windows.Forms.MessageBox.Show("No active code is open");
                return new List<TableRecord>();
            }

            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            Collector collector = new Collector(activeCodeModel, debugPane);

            return collector.ParseFileCodeModel();
        }


    }
}
