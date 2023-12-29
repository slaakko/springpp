program Arrow;
uses
  System.Graphics.Primitive,
  System.Graphics;
var
  x, y: integer;
  bm: Bitmap;
  graphics: Graphics;
  arrow: Arrow;
  arrowPoints: PointArray;
begin
  x := MMToPixels(100, dpiX);
  y :=  MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  arrowPoints := new Point[2];
  arrowPoints[0] := new Point(60, 60);
  arrowPoints[1] := new Point(80, 60);
  arrow := new Arrow(arrowPoints);
  arrow.Draw(graphics);
  bm.Save('C:/work/springpp/prog/arrow.png');
end.
