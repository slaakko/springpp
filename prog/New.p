program New;
type
  Foo = object
  end;
var 
  foo, bar: Foo;
begin
  foo := new Foo();
  bar := new Foo;
end.
