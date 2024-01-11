program ConstParam;

procedure Foo(const p: integer);
begin
  p := 1;
end;

begin
  Foo(1);
end.
