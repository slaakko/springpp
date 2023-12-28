program Virtual;
type
  Shape = object
    constructor();
    procedure Draw(); virtual;
  end;

  Circle = object(Shape)
    constructor();
    procedure Draw(); override;
  end;

var
  s: Shape;
  c: Shape;

constructor Shape();
begin
end;

procedure Shape.Draw();
begin
  Writeln('Shape.Draw');
end;

constructor Circle();
begin
end;

procedure Circle.Draw();
begin
  base.Draw();
  Writeln('Circle.Draw');
end;

begin
  s := new Shape();
  s.Draw();
  c := new Circle();
  c.Draw();
end.
