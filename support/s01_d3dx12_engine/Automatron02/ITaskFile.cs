using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron02
{
   interface ITaskFile : ITask
   {
      string FilePath
      {
         get;
      }
   }
}
