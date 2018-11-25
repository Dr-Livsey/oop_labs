using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.CodeDom;
using System.Text.RegularExpressions;
using EnvDTE80;
using EnvDTE;

/*TODO:
    /*
     1. прототип
     2. количество строк
     3. количество строк без учета пустых и комментариев
     4. количество ключевых слов (для языков Python, C, C++, C#, F# перечень представлен в Приложении В)
     5. для каждого класса – число public/protected/private полей и число public/ protected/private методов. *DONE*
    */

namespace VSExtension
{
    class Collector
    {
        private OutputWindowPane    debugPane; /*Only for debug*/
        private FileCodeModel2      sourceItem;

        public Collector(FileCodeModel2 sourceItem, OutputWindowPane debugPane)
        {
            this.debugPane  =  debugPane ?? throw new ArgumentNullException(nameof(debugPane));
            this.sourceItem =  sourceItem ?? throw new ArgumentNullException(nameof(sourceItem));
        }

        private Collector() { }

        public List<TableRecord> ParseFileCodeModel()
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            List<TableRecord> tableList = new List<TableRecord>();

            foreach (CodeElement2 codeElement in sourceItem.CodeElements)
            {
                /*Class*/
                if (codeElement.Kind == vsCMElement.vsCMElementClass)
                {
                    CodeClass2 classItem = codeElement as CodeClass2 ?? throw new ArgumentNullException();

                    List<TableRecord> classTRec = parseClass(classItem);
                    tableList.AddRange(classTRec);
                }
                /*Parse struct functions*/
                else if (codeElement.Kind == vsCMElement.vsCMElementStruct)
                {
                    CodeStruct2 structItem = codeElement as CodeStruct2 ?? throw new ArgumentNullException();

                    debugPane.OutputString("Struct methods: " + structItem.FullName + "\n");
                    foreach (CodeElement2 field in structItem.Members)
                    {
                        if (field.Kind == vsCMElement.vsCMElementFunction)
                        {
                              TableRecord structTRec = parseFuntion(field as CodeFunction2);
                              tableList.Add(structTRec);
                        }
                    }
                }
                /*Parse single functions*/
                else if (codeElement.Kind == vsCMElement.vsCMElementFunction)
                {
                     TableRecord funRec = parseFuntion(codeElement as CodeFunction2);
                     tableList.Add(funRec);
                }
            }

            return tableList;
        }

        public TableRecord parseFuntion(CodeFunction2 funItem)
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            if (funItem == null)
                throw new ArgumentNullException("CodeFunction2 == null in parseFuntion");

            TableRecord functionTRec = new TableRecord();

            string[] functionItems = getItemSourceCode(funItem as CodeElement2);

            string prototype = functionItems[0];
            string implemention = functionItems[1];

            debugPane.OutputString("Parsing ---> " + prototype + "\n");
 
            int allLines = linesAmount(implemention);

            deleteComments(ref implemention);
            deleteEmptyStrings(ref implemention);

            int clearLines = linesAmount(implemention);

            int keyWords = keyWordsAmount(functionItems[1]);

            functionTRec.ItemName = prototype;
            functionTRec.LinesAmount = allLines.ToString();
            functionTRec.LinesAmountWithoutComments = clearLines.ToString();
            functionTRec.KeyWordsAmount = keyWords.ToString();
            functionTRec.ClassMethodsAmount = "-";
            functionTRec.ClassFieldsAmount = "-";

            //debugPane.OutputString(implemention.ToString() + "\n");

            return functionTRec;
        }

        public int linesAmount(string source)
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            return Regex.Matches(source, @"\n").Count;
        }

        public int keyWordsAmount(string clearSource)
        {
            deleteQStrings(ref clearSource);
            deleteComments(ref clearSource);

            string pattern = @"\b(alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|
            case|catch|char|
            char16_t|char32_t|class|compl|const|constexpr|const_cast|continue|decltype|
            default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|
            float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|
            not_eq|nullptr|operator|or|or_eq|private|protected|public|register|
            reinterpret_cast|return|short|signed|sizeof|static|static_assert|static_cast|
            struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|
            typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xorxor_eq)\b";

            return Regex.Matches(clearSource, pattern).Count;
        }

        private void deleteComments(ref string dirtySource)
        {
            string backSlashCommentPattern = @"(?:\/\/(?:[^\\\r\n]|\\+\r\n|\\+)*)";
            string starsCommentPattern = @"(?:(?:\/\*(?:[^*]|[\r\n]|(?:\*+(?:[^*/]|[\r\n])))*\*+\/))";

            string commentsPattern = starsCommentPattern + "|" + backSlashCommentPattern;

            Regex commentRegex = new Regex(commentsPattern);

            dirtySource = commentRegex.Replace(dirtySource, "");
        }

        private void deleteQStrings(ref string dirtySource)
        {
            string dbQuotedPattern = "(?:((?<![\\\\])\\\")(?:[^\\\\\\\"\\r\\n]|\\\\[^\\r\\n]|\\\\\\r\\n)*\\1)";
            string sQuotedPattern = "(?:((?<![\\\\])\\\')(?:[^\\\\\\\'\\r\\n]|\\\\[^\\r\\n]|\\\\\\r\\n)*\\1)";

            Regex dbQuotedRegex = new Regex(dbQuotedPattern);
            Regex sQuotedRegex = new Regex(sQuotedPattern);

            dirtySource = dbQuotedRegex.Replace(dirtySource, "");
            dirtySource = sQuotedRegex.Replace(dirtySource, "");
        }

        private void deleteEmptyStrings(ref string dirtySource)
        {
            string eatSpacePattern = @"(?:(?:\r\n\s*){2,})";

            Regex EatSpace = new Regex(eatSpacePattern);

            dirtySource = EatSpace.Replace(dirtySource, "\n");
        }


        private string[] getItemSourceCode(CodeElement2 codeElement)
        {
            if (codeElement == null)
                throw new ArgumentNullException("codeElement == null in getItemSourceCode");

            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            EditPoint begin = codeElement.StartPoint.CreateEditPoint();
            EditPoint end = codeElement.EndPoint.CreateEditPoint();

            string fullSource = begin.GetText(end);

            // Find the first open curly brace
            int openCurlyBracePos = fullSource.IndexOf('{');
            int closeCurlyBracePos = fullSource.LastIndexOf("}");

            if (openCurlyBracePos == - 1)
                openCurlyBracePos = fullSource.IndexOf(';');

            string prototype = String.Empty;
            string implementation = String.Empty;

            if (openCurlyBracePos > -1)
            {
                if (closeCurlyBracePos > -1)
                {
                    implementation = fullSource.Substring(openCurlyBracePos, closeCurlyBracePos - openCurlyBracePos + 1);
                }
                else implementation = "{}";

                prototype = fullSource.Substring(0, openCurlyBracePos);

                implementation.Trim();
                prototype.Trim();
            }

            return new string[]{ prototype, implementation };
        }

        public List<TableRecord> parseClass(CodeClass2 classItem)
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            Dictionary<string, int> methodsDictionary = collectMethods(classItem);
            Dictionary<string, int> fieldsDictionary = collectFields(classItem);


            List<TableRecord> tRecList = new List<TableRecord>();
            TableRecord       classTableRecord = new TableRecord();

            classTableRecord.ItemName = "Class " + classItem.FullName;
            classTableRecord.ClassMethodsAmount = "+ " + methodsDictionary["public"].ToString() + " | " +
                                                  "- " + methodsDictionary["private"].ToString() + " | " +
                                                  "# " + methodsDictionary["protected"].ToString();

            classTableRecord.ClassFieldsAmount =  "+ " + fieldsDictionary["public"].ToString() + " | " +
                                                  "- " + fieldsDictionary["private"].ToString() + " | " +
                                                  "# " + fieldsDictionary["protected"].ToString();
            classTableRecord.KeyWordsAmount = "-";
            classTableRecord.LinesAmount = "-";
            classTableRecord.LinesAmountWithoutComments = "-";

            tRecList.Add(classTableRecord);

            foreach (CodeElement2 item in classItem.Members)
            {
                if (item.Kind == vsCMElement.vsCMElementFunction)
                {
                    CodeFunction2 codeFunction = item as CodeFunction2 ?? throw new ArgumentException();

                    /* Add table record to list. */
                     TableRecord funTRec = parseFuntion(codeFunction);
                     tRecList.Add(funTRec);
                }
            }

            return tRecList;
        }

        public Dictionary<string, int> collectMethods(CodeClass2 classItem)
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            Dictionary<string, int> dict = new Dictionary<string, int>()
            {
                { "public",     0},
                { "private",    0},
                { "protected",  0}
            };

            debugPane.OutputString("\nClass methods: " + classItem.FullName + "\n");
            foreach (CodeElement2 field in classItem.Members)
            {
                if (field.Kind == vsCMElement.vsCMElementFunction)
                {
                    CodeFunction2 codeFunction = field as CodeFunction2;

                    if (codeFunction.Access == vsCMAccess.vsCMAccessPublic)
                    {
                        //debugPane.OutputString("\tpublic: \n");
                       // debugPane.OutputString("\t\t " + codeFunction.FullName + "\n");

                        dict["public"]++;
                    }
                    else if (codeFunction.Access == vsCMAccess.vsCMAccessPrivate)
                    {
                        //debugPane.OutputString("\tprivate: \n");
                        //debugPane.OutputString("\t\t " + codeFunction.FullName + "\n");

                        dict["private"]++;
                    }
                    else if (codeFunction.Access == vsCMAccess.vsCMAccessProtected)
                    {
                        //debugPane.OutputString("\tprotected: \n");
                        //debugPane.OutputString("\t\t " + codeFunction.FullName + "\n");

                        dict["protected"]++;
                    }
                }
            }

            return dict;
        }
        public Dictionary<string, int> collectFields(CodeClass2 classItem)
        {
            Microsoft.VisualStudio.Shell.ThreadHelper.ThrowIfNotOnUIThread();

            Dictionary<string, int> dict = new Dictionary<string, int>()
            {
                { "public",     0},
                { "private",    0},
                { "protected",  0}
            };

            debugPane.OutputString("\nClass fields: " + classItem.FullName + "\n");
            foreach (CodeElement2 field in classItem.Members)
            {
                if (field.Kind == vsCMElement.vsCMElementVariable)
                {
                    CodeVariable2 codeVariable = field as CodeVariable2;

                    if (codeVariable.Access == vsCMAccess.vsCMAccessPublic)
                    {
                        debugPane.OutputString("\tpublic: \n");
                        debugPane.OutputString("\t\t " + codeVariable.FullName + "\n");

                        dict["public"]++;
                    }
                    else if (codeVariable.Access == vsCMAccess.vsCMAccessPrivate)
                    {
                        debugPane.OutputString("\tprivate: \n");
                        debugPane.OutputString("\t\t " + codeVariable.FullName + "\n");

                        dict["private"]++;
                    }
                    else if (codeVariable.Access == vsCMAccess.vsCMAccessProtected)
                    {
                        debugPane.OutputString("\tprotected: \n");
                        debugPane.OutputString("\t\t " + codeVariable.FullName + "\n");

                        dict["protected"]++;
                    }
                }
            }

            return dict;
        }
    }
}
