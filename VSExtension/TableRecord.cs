using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VSExtension
{
    /*
     - прототип
     - количество строк
     - количество строк без учета пустых и комментариев
     - количество ключевых слов (для языков Python, C, C++, C#, F# перечень представлен в Приложении В)
     - для каждого класса – число public/protected/private полей и число public/ protected/private методов.
    */
    public class TableRecord
    {
        public string ItemName { get; set; }
        public string LinesAmount { get; set; }
        public string LinesAmountWithoutComments { get; set; }
        public string KeyWordsAmount { get; set; }
        public string ClassFieldsAmount { get; set; }
        public string ClassMethodsAmount { get; set; }
    }
}
