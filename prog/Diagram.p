program Diagram;

uses System.Graphics.Primitive, System.Graphics, Syntax;

var
  x, y: integer;
  bm: Bitmap;
  graphics: Graphics;
  diagram: Diagram;
  
begin
  x := MMToPixels(100, dpiX);
  y := MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  diagram := new Diagram(new DiagramTitle('term'));
  diagram.Add(new RuleBox('factor'));
  bm.Save('C:/work/springpp/prog/Diagram.png');
end.