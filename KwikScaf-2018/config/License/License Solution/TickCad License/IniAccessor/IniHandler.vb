Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Text
Imports System.Collections.Generic

Public Class IniFile
    Public path As String

    <DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileStringW", SetLastError:=True, CharSet:=CharSet.Unicode, ExactSpelling:=True, CallingConvention:=CallingConvention.StdCall)> _
    Private Shared Function GetPrivateProfileString(ByVal lpAppName As String, ByVal lpKeyName As String, ByVal lpDefault As String, ByVal lpReturnString As String, ByVal nSize As Integer, ByVal lpFilename As String) As Integer
    End Function
    <DllImport("KERNEL32.DLL", EntryPoint:="WritePrivateProfileStringW", SetLastError:=True, CharSet:=CharSet.Unicode, ExactSpelling:=True, CallingConvention:=CallingConvention.StdCall)> _
    Private Shared Function WritePrivateProfileString(ByVal lpAppName As String, ByVal lpKeyName As String, ByVal lpString As String, ByVal lpFilename As String) As Integer
    End Function

    Public Sub New(ByVal INIPath As String)
        path = INIPath
    End Sub
    Public Sub IniWriteValue(ByVal Section As String, ByVal Key As String, ByVal Value As String)
        WritePrivateProfileString(Section, Key, Value, Me.path)
    End Sub
    Public Function IniReadValue(ByVal Section As String, ByVal Key As String) As String
        Dim result As New String(" "c, 50000)
        GetPrivateProfileString(Section, Key, "", result, 50000, Me.path)
        Return result

    End Function
    Public Function IniGetCategories() As List(Of String)
        Dim returnString As New String(" "c, 65536)
        GetPrivateProfileString(Nothing, Nothing, Nothing, returnString, 65536, Me.path)
        Dim result As New List(Of String)(returnString.Split(ControlChars.NullChar))
        result.RemoveRange(result.Count - 2, 2)
        Return result
    End Function
    Public Function IniGetKeys(ByVal category As String) As List(Of String)
        Dim returnString As New String(" "c, 32768)
        GetPrivateProfileString(category, Nothing, Nothing, returnString, 32768, Me.path)
        Dim result As New List(Of String)(returnString.Split(ControlChars.NullChar))
        result.RemoveRange(result.Count - 2, 2)
        Return result
    End Function

    Function GetIniFileString(ByVal gInifile As String, ByVal section As String, ByVal variable As String) As String

        Dim objIni As New IniFile(gInifile)
        If variable <> "" Then
            GetIniFileString = objIni.IniReadValue(section, variable)
            GetIniFileString = GetIniFileString.Trim()
        Else
            Dim temp As String = String.Empty

            Dim tempcol As System.Collections.Generic.List(Of String)
            tempcol = objIni.IniGetKeys(section)
            For Each ts As String In tempcol
                temp = temp + ts + ";"
            Next
            GetIniFileString = temp.Trim()
        End If

        GetIniFileString.Replace(System.Environment.NewLine, "")

    End Function

    Function SetIniFileString(ByVal gInifile As String, ByVal section As String, ByVal SetvariableName As String, ByVal setVariableValue As String) As String

        Dim objSetIni As New IniFile(gInifile)
        If SetvariableName <> "" Then

            objSetIni.IniWriteValue(section, SetvariableName, setVariableValue)
            'SetIniFileString = SetIniFileString.Trim()
        Else
            Dim temp As String = String.Empty

            Dim tempcol As System.Collections.Generic.List(Of String)
            tempcol = objSetIni.IniGetKeys(section)
            For Each ts As String In tempcol
                temp = temp + ts + ";"
            Next
            SetIniFileString = temp.Trim()
        End If

        'SetIniFileString.Replace(System.Environment.NewLine, "")

    End Function

End Class
