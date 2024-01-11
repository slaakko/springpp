program idlist;

uses System.Graphics.Primitive, System.Graphics, Syntax;

type
  DiagramArray = array of Diagram;

var
  diagrams: DiagramArray;
  diagramCount: integer;
  x, y: integer;
  initialBitmap: Bitmap;
  initialGraphics: Graphics;

procedure AddDiagram(diagram: Diagram);
var
  newLength, i: integer;
  newDiagrams: DiagramArray;
begin
  if diagramCount = diagrams.Length then
  begin
    if diagrams.Length < 4 then newLength := 4 else newLength := 2 * diagrams.Length;
    newDiagrams := new Diagram[newLength];
    for i := 0 to diagramCount - 1 do newDiagrams[i] := diagrams[i];
    diagrams := newDiagrams;
  end;
  diagrams[diagramCount] := diagram;
  diagramCount := Succ(diagramCount);
end;

procedure MakeDiagrams();

  procedure MakeIdentifierListDiagram();
  var
    title: DiagramTitle;
    diagram: Diagram;
    branch1, branch2: Branch;
    out: Out;
    identifier: RuleBox;
    comma: Terminal;
  begin
    title := new DiagramTitle('identifier-list');
    diagram := new Diagram(title, 'identifier-list.png');
    branch1 := new Branch();
    diagram.AddShape(0, 1, branch1);
    identifier := new RuleBox('identifier');
    diagram.AddShape(0, 2, identifier);
    comma := new Terminal(',');
    diagram.AddShape(1, 2, comma);
    branch2 := new Branch();
    diagram.AddShape(0, 3, branch2);
    out := new Out();
    diagram.AddShape(0, 4, out);
    diagram.AddConnector(new SyntaxArrow(title, identifier, east, noDir));
    diagram.AddConnector(new SyntaxArrow(identifier, out, east, noDir));
    diagram.AddConnector(new SyntaxArrow(branch2, comma, south, west));
    diagram.AddConnector(new SyntaxArrow(comma, branch1, west, north));
    AddDiagram(diagram);
  end;

begin
  MakeIdentifierListDiagram(); 
end;

procedure MeasureDiagrams();
var
  i: integer;
  diagram: Diagram;
begin
  for i := 0 to diagramCount - 1 do 
  begin
    diagram := diagrams[i];
    diagram.Measure(initialGraphics);
  end;
end;

procedure PrintDiagrams();
var
  i: integer;
  diagram: Diagram;
begin
  for i := 0 to diagramCount - 1 do 
  begin
    diagram := diagrams[i];
    diagram.Print();
  end;
end;

procedure DrawDiagrams();
var
  i: integer;
  diagram: Diagram;
  bm: Bitmap;
  x, y: integer;
  graphics: Graphics;
  dir, filePath: string;
begin
  dir := 'C:/work/springpp/syntax/';
  for i := 0 to diagramCount - 1 do 
  begin
    diagram := diagrams[i];
    x := MMToPixels(diagram.bounds.size.w, dpiX);
    y := MMToPixels(diagram.bounds.size.h, dpiY);
    bm := new Bitmap(x, y);
    graphics := bm.GetGraphics();
    diagram.Draw(graphics);
    filePath := dir + diagram.fileName;
    bm.Save(filePath);
  end;
end;

begin
  x := MMToPixels(100, dpiX);
  y := MMToPixels(100, dpiY);
  initialBitmap := new Bitmap(x, y);
  initialGraphics := initialBitmap.GetGraphics();
  diagrams := new Diagram[0];
  diagramCount := 0;
  MakeDiagrams();
  MeasureDiagrams();
  PrintDiagrams();
  DrawDiagrams();
end.  
