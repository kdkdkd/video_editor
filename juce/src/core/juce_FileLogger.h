/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_FILELOGGER_JUCEHEADER__
#define __JUCE_FILELOGGER_JUCEHEADER__

#include "juce_Logger.h"
#include "../io/files/juce_File.h"
#include "../memory/juce_ScopedPointer.h"


//==============================================================================
/**
    A simple implemenation of a Logger that writes to a file.

    @see Logger
*/
class JUCE_API  FileLogger  : public Logger
{
public:
    //==============================================================================
    /** Creates a FileLogger for a given file.

        @param fileToWriteTo    the file that to use - new messages will be appended
                                to the file. If the file doesn't exist, it will be created,
                                along with any parent directories that are needed.
        @param welcomeMessage   when opened, the logger will write a header to the log, along
                                with the current date and time, and this welcome message
        @param maxInitialFileSizeBytes  if this is zero or greater, then if the file already exists
                                but is larger than this number of bytes, then the start of the
                                file will be truncated to keep the size down. This prevents a log
                                file getting ridiculously large over time. The file will be truncated
                                at a new-line boundary. If this value is less than zero, no size limit
                                will be imposed; if it's zero, the file will always be deleted. Note that
                                the size is only checked once when this object is created - any logging
                                that is done later will be appended without any checking
    */
    FileLogger (const File& fileToWriteTo,
                const String& welcomeMessage,
                const int maxInitialFileSizeBytes = 128 * 1024);

    /** Destructor. */
    ~FileLogger();

    //==============================================================================
    void logMessage (const String& message);

    const File getLogFile() const               { return logFile; }

    //==============================================================================
    /** Helper function to create a log file in the correct place for this platform.

        On Windows this will return a logger with a path such as:
        c:\\Documents and Settings\\username\\Application Data\\[logFileSubDirectoryName]\\[logFileName]

        On the Mac it'll create something like:
        ~/Library/Logs/[logFileName]

        The method might return 0 if the file can't be created for some reason.

        @param logFileSubDirectoryName      if a subdirectory is needed, this is what it will be called -
                                            it's best to use the something like the name of your application here.
        @param logFileName                  the name of the file to create, e.g. "MyAppLog.txt". Don't just
                                            call it "log.txt" because if it goes in a directory with logs
                                            from other applications (as it will do on the Mac) then no-one
                                            will know which one is yours!
        @param welcomeMessage               a message that will be written to the log when it's opened.
        @param maxInitialFileSizeBytes      (see the FileLogger constructor for more info on this)
    */
    static FileLogger* createDefaultAppLogger (const String& logFileSubDirectoryName,
                                               const String& logFileName,
                                               const String& welcomeMessage,
                                               const int maxInitialFileSizeBytes = 128 * 1024);

private:
    //==============================================================================
    File logFile;
    CriticalSection logLock;

    void trimFileSize (int maxFileSizeBytes) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileLogger);
};


#endif   // __JUCE_FILELOGGER_JUCEHEADER__
