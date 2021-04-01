using System;
using System.Collections.Generic;
using System.Text;
using Softing.OPCToolbox;
using System.IO;

namespace DemoClient.Helper
{
    public static class Options
    {
        private static System.Configuration.Configuration config = System.Configuration.ConfigurationManager.OpenExeConfiguration(System.Configuration.ConfigurationUserLevel.None);

        public static EnumTraceGroup Error
        {
            get
            {
                return (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup),
                                       System.Configuration.ConfigurationManager.AppSettings["ErrorCalls"].ToString());
            }
            set
            {
                config.AppSettings.Settings["ErrorCalls"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static EnumTraceGroup Warning
        {
            get
            {
                return (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup),
                                       System.Configuration.ConfigurationManager.AppSettings["WarningCalls"].ToString());
            }
            set
            {
                config.AppSettings.Settings["WarningCalls"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static EnumTraceGroup Info
        {
            get
            {
                return (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup),
                                       System.Configuration.ConfigurationManager.AppSettings["InfoCalls"].ToString());
            }
            set
            {
                config.AppSettings.Settings["InfoCalls"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }

        }

        public static EnumTraceGroup Debug
        {
            get
            {
                return (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup),
                                       System.Configuration.ConfigurationManager.AppSettings["DebugCalls"].ToString());
            }
            set
            {
                config.AppSettings.Settings["DebugCalls"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static string TraceFile
        {
            get
            {
                return System.Configuration.ConfigurationManager.AppSettings["TraceFile"].ToString();
            }
            set
            {
                config.AppSettings.Settings["TraceFile1"].Value = value;
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }

        }

        //public static string TraceFile2
        //{
        //    get
        //    {
        //        return System.Configuration.ConfigurationManager.AppSettings["TraceFile2"].ToString();
        //    }
        //    set
        //    {
        //        config.AppSettings.Settings["TraceFile2"].Value = value;
        //        config.Save(System.Configuration.ConfigurationSaveMode.Modified);
        //        System.Configuration.ConfigurationManager.RefreshSection("appSettings");

        //    }
        //}

        public static bool PureTime
        {
            get
            {
                bool pureTime = true;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["PureTime"].ToString(), out pureTime);
                return pureTime;
            }
            set
            {
                config.AppSettings.Settings["PureTime"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }

        }

        public static bool WithDateTime
        {
            get
            {
                bool withDateTime = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["WithDateTime"].ToString(), out withDateTime);
                return withDateTime;
            }
            set
            {
                config.AppSettings.Settings["WithDateTime"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool PureItemValue
        {
            get
            {
                bool pureItemValue = true;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["PureItemValue"].ToString(), out pureItemValue);
                return pureItemValue;
            }
            set
            {
                config.AppSettings.Settings["PureItemValue"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool VTItemValue
        {
            get
            {
                bool vTItemValue = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["VTItemValue"].ToString(), out vTItemValue);
                return vTItemValue;
            }
            set
            {
                config.AppSettings.Settings["VTItemValue"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool DecItemValue
        {
            get
            {
                bool decItemValue = true;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["DecItemValue"].ToString(), out decItemValue);
                return decItemValue;
            }
            set
            {
                config.AppSettings.Settings["DecItemValue"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool HexItemValue
        {
            get
            {
                bool hexItemValue = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["HexItemValue"].ToString(), out hexItemValue);
                return hexItemValue;
            }
            set
            {
                config.AppSettings.Settings["HexItemValue"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool SortDa
        {
            get
            {
                bool sortDa = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["SortDa"].ToString(), out sortDa);
                return sortDa;
            }
            set
            {
                config.AppSettings.Settings["SortDa"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool SortAe
        {
            get
            {
                bool sortAe = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["SortAe"].ToString(), out sortAe);
                return sortAe;
            }
            set
            {
                config.AppSettings.Settings["SortAe"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool ConnMonitor
        {
            get
            {
                bool connMonitor = false;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["ConnMonitor"].ToString(), out connMonitor);
                return connMonitor;
            }
            set
            {
                config.AppSettings.Settings["ConnMonitor"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static bool TraceCalls
        {
            get
            {
                bool traceCalls = true;
                Boolean.TryParse(System.Configuration.ConfigurationManager.AppSettings["TraceCalls"].ToString(), out traceCalls);
                return traceCalls;
            }
            set
            {
                config.AppSettings.Settings["TraceCalls"].Value = value.ToString();
                config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                System.Configuration.ConfigurationManager.RefreshSection("appSettings");
            }
        }

        public static List<FilesStruct> LastSavedFiles
        {
            get
            {
                List<FilesStruct> lastSavedFiles = new List<FilesStruct>();
                if (File.Exists(System.Configuration.ConfigurationManager.AppSettings["FileSaved1"].ToString()))
                {
                    lastSavedFiles.Add(new FilesStruct(Path.GetFileNameWithoutExtension(System.Configuration.ConfigurationManager.AppSettings["FileSaved1"].ToString()),
                                        System.Configuration.ConfigurationManager.AppSettings["FileSaved1"].ToString()));
                }
                if (File.Exists(System.Configuration.ConfigurationManager.AppSettings["FileSaved2"].ToString()))
                {
                    lastSavedFiles.Add(new FilesStruct(Path.GetFileNameWithoutExtension(System.Configuration.ConfigurationManager.AppSettings["FileSaved2"].ToString()),
                                        System.Configuration.ConfigurationManager.AppSettings["FileSaved2"].ToString()));
                }
                if (File.Exists(System.Configuration.ConfigurationManager.AppSettings["FileSaved3"].ToString()))
                {
                    lastSavedFiles.Add(new FilesStruct(Path.GetFileNameWithoutExtension(System.Configuration.ConfigurationManager.AppSettings["FileSaved3"].ToString()),
                                        System.Configuration.ConfigurationManager.AppSettings["FileSaved3"].ToString()));
                }
                if (File.Exists(System.Configuration.ConfigurationManager.AppSettings["FileSaved4"].ToString()))
                {
                    lastSavedFiles.Add(new FilesStruct(Path.GetFileNameWithoutExtension(System.Configuration.ConfigurationManager.AppSettings["FileSaved4"].ToString()),
                                        System.Configuration.ConfigurationManager.AppSettings["FileSaved4"].ToString()));
                }
                return lastSavedFiles;
            }

            set
            {
                List<FilesStruct> lastSavedFiles = value;
                int i = 1;
                foreach (FilesStruct savedFile in lastSavedFiles)
                {
                    switch (i)
                    {
                        case 1:
                            config.AppSettings.Settings["FileSaved1"].Value = savedFile.FilePath;
                            break;
                        case 2:
                            config.AppSettings.Settings["FileSaved2"].Value = savedFile.FilePath;
                            break;
                        case 3:
                            config.AppSettings.Settings["FileSaved3"].Value = savedFile.FilePath;
                            break;
                        case 4:
                            config.AppSettings.Settings["FileSaved4"].Value = savedFile.FilePath;
                            break;
                    }
                    config.Save(System.Configuration.ConfigurationSaveMode.Modified);
                    System.Configuration.ConfigurationManager.RefreshSection("appSettings");
                    i++;
                }
            }
        }
    }

    /// <summary>
    /// Last saved files structure
    /// </summary>
    public struct FilesStruct
    {
        private string fileName;
        public string FileName
        {
            get { return fileName; }
            set { fileName = value; }
        }

        private string filePath;
        public string FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        public FilesStruct(string fName, string fPath)
        {
            fileName = fName;
            filePath = fPath;
        }
    }
}


