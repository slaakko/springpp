program Line;
uses
  System.Graphics.Primitive,
  System.Graphics;
var
  bm: Bitmap;
  x, y: integer;
  graphics: Graphics;
  arrow: Arrow;
  font: Font;
  size: Size;
  textX, textY: real;
  arrowPoints: PointArray;
begin
  x := MMToPixels(100, dpiX);
  y :=  MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  graphics.DrawLine(blackPen, new Point(0, 0), new Point(100, 100));
  graphics.DrawRectangle(blackPen, new Rect(new Point(10, 10), new Size(80, 80)));
  font := new Font('Arial', 10, regularFontStyle);
  size := graphics.MeasureString('foobar', font);
  textX := 50 - size.w / 2;
  textY := 50 - size.h / 2;
  graphics.DrawString('foobar', font, blackBrush, new Point(textX, textY));
  graphics.DrawEllipse(blackPen, new Rect(new Point(10, 10), new Size(10, 10)));
  graphics.DrawArc(blackPen, new Rect(new Point(40, 40), new Size(10, 10)), 0, 90);
  arrowPoints := new Point[2];
  arrowPoints[0] := new Point(60, 60);
  arrowPoints[1] := new Point(80, 60);
  arrow := new Arrow(arrowPoints);
  arrow.Draw(graphics);
  bm.Save('C:/work/springpp/prog/line.png');
end.
