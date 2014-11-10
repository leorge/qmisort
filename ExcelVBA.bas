Attribute VB_Name = "modMacros"
Option Explicit

'   How to make a data file
'       for 'eval' macro
'           $ src/evaluate 10 20 <options> | src/matrix.awk -v OFS="\t" > file
'       for 'nmemb' macro
'           $ N=xxx; src/random.awk | Debug/test_sort -N $N -V 2 <options> | \
'           > src/nmemb.awk -v OFS="\t" > file
'       for 'rate' macro
'           $ N=xxx; src/random.awk | Debug/test_sort -N $N -V 2 <options> | \
'           > src/rate.awk -v OFS="\t" > file
'
'   How to install this macro
'       (a) Open Microsoft Excel(TM) 2003 or later.
'       (b) Press Alt + F11 key to open VBE(Visual BASIC Editor).
'       (c) Click right button on a project tree.
'       (d) Select the file 'ExcelVBA.bas'
'       (e) Press Alt + F4 key to close VBE.
'
'   How to use macros
'       (a) Press Alt + F8 key to invoke a macro.
'       (b) Select a macro
'       (c) Click 'Run' button.
'

Private Sub GetAdrs(ByRef Cell As Range, ByRef X As String, ByRef Y As Long)
    Dim Where As String: Where = Cell.Address
    Dim V As Variant: V = Split(Mid(Where, 2), "$")
    X = CStr(V(0)): Y = CLng(V(1))
End Sub

Private Function OpenFile(ByRef X As String, ByRef Y As Long) As Boolean
    Dim Fname As String
    With Application.FileDialog(msoFileDialogFilePicker)
        .Filters.Add "All files", "*.*"
        .AllowMultiSelect = False
        If .Show = 0 Then Exit Function
        Fname = .SelectedItems(1)  ' get a file name to open
    End With
'    If Fname = "" Then Exit Function
    '=====  Close a workbook with the same name =====
    Dim objFs As Object: Set objFs = CreateObject("Scripting.FileSystemObject")
    On Error Resume Next    ' get a window if exist
    Dim obj As Object: Set obj = Windows(objFs.GetFileName(Fname))
    On Error GoTo 0
    If Not obj Is Nothing Then
        obj.Close False: Set obj = Nothing  ' close a workbook
    End If
    '=====  Open a file and autofit rows =====
    Workbooks.OpenText FileName:=Fname, Tab:=True
    GetAdrs Range("A1").End(xlDown), X, Y
    Dim DummyY As Long: GetAdrs Range("A1").End(xlToRight), X, DummyY
    Columns("A:" & X).EntireColumn.AutoFit
    OpenFile = True
End Function

Private Sub matrix(Optional ByVal Sort As Boolean)
    Const Wd As Single = 324    ' Width of chart
    Dim Log10 As Double: Log10 = Log(10)
    Dim X As String, TopY As Long
    If Not OpenFile(X, TopY) Then Exit Sub
    Dim SheetName As String: SheetName = ActiveSheet.Name
    Dim Lft As Single: Lft = Columns("A:" & X).Width    ' Left of chart
    Range("A1").Select
    Dim Scount As Integer
    Do
        ' Sort a table
        
        Dim Anchor As Range: Set Anchor = ActiveCell
        Dim DummyX As String: GetAdrs Anchor, DummyX, TopY
        Dim Bottom As Range: Set Bottom = ActiveCell.End(xlDown)
        Dim BottomY As Long: GetAdrs Bottom, DummyX, BottomY
        Dim H As Single: H = Rows(TopY & ":" & BottomY).Height
        Dim Title As String: Title = Anchor.Text: Anchor.ClearContents
        ActiveSheet.Range("A" & TopY & ":" & X & BottomY).Select
        Dim Area As Range: Set Area = Selection
        If Sort Then Area.Sort Key1:=Range("L2"), Header:=xlYes
        
        ' Add a chart
        
        Charts.Add
        ActiveChart.ChartType = xlLine
        ActiveChart.SetSourceData Source:=Sheets(SheetName).Range("A" & Anchor.Row & ":" & X & Bottom.Row) _
            , PlotBy:=xlRows
        ActiveChart.Location Where:=xlLocationAsObject, Name:=SheetName
        With ActiveChart
            With .Axes(xlValue)
                .ScaleType = xlLogarithmic
                .MinimumScale = 10 ^ (Int(Log(WorksheetFunction.Min( _
                    Range("A" & TopY & ":" & X & BottomY)) + 1) / Log10))   ' Add 1 to prevent Log(0)
            End With
            Dim TitleLeft As Single: TitleLeft = .Legend.Left
            .Legend.Height = .PlotArea.Height
            .SetElement (msoElementChartTitleCenteredOverlay)   ' Excel 2007 or later
'            .HasTitle = True                                    ' Excel 2003
            With .ChartTitle
                .Text = Title
                .Left = TitleLeft
            End With
        End With
        
        ' locate the chart
        
        Scount = Scount + 1
        With ActiveSheet.Shapes(Scount)
            .Top = Anchor.Top
            .Height = H
            .Left = Area.Width
        End With
        Anchor = Title  ' Restore
        
        ' Next table
        
        Bottom.End(xlDown).Select
        DoEvents    ' to abort
    Loop While Not IsEmpty(ActiveCell)
    ActiveCell.End(xlUp).Select
End Sub

Public Sub eval()
    matrix False
End Sub

Public Sub rate()
    matrix
End Sub

Public Sub nmemb()
    Dim X As String, Y As Long
    If Not OpenFile(X, Y) Then Exit Sub
    '===== sort by sum  =====
    With ActiveSheet.Sort
        .SortFields.Clear
        .SortFields.Add Key:=Range(X & "2:" & X & Y)
        .SortFields.Add Key:=Range("B2:B" & Y)
        .SetRange Range("A1:" & X & Y)
        .Header = xlYes
        .MatchCase = False
        .Orientation = xlTopToBottom
        .SortMethod = xlPinYin
        .Apply
    End With
    '===== Add a chart  =====
    Range("A1").Select
    Dim Xtail As String, DummyY As Long
    GetAdrs ActiveCell.End(xlToRight).Offset(0, -1), Xtail, DummyY
    ActiveSheet.Shapes.AddChart(xl3DArea, 0, Cells(Y + 2, 1).Top, _
        Range("A1:" & Xtail & "1").Width, Rows("1:15").Height).Select
    With ActiveChart
        .SetSourceData Source:=ActiveSheet.Range("A2:" & Xtail & Y)
        .Axes(xlSeries).Delete
        .ChartArea.Copy
    End With
    ActiveSheet.Shapes(1).Name = "Linear"
    '===== Duplicate the chart  =====
    Application.DisplayAlerts = False
    Range(X & Y + 2).Select
    ActiveSheet.Paste
    ActiveChart.Axes(xlValue).ScaleType = xlLogarithmic
    ActiveSheet.Shapes(2).Name = "Log"
    Range("A1").Select
End Sub

