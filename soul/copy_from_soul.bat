@echo off
xcopy /F /Y ..\..\soul-mod\soul\ast\*.cppm ast\
xcopy /F /Y ..\..\soul-mod\soul\ast\*.cpp ast\

xcopy /F /Y ..\..\soul-mod\soul\lexer\*.cppm lexer\
xcopy /F /Y ..\..\soul-mod\soul\lexer\*.cpp lexer\

xcopy /F /Y ..\..\soul-mod\soul\parser\*.cppm parser\
xcopy /F /Y ..\..\soul-mod\soul\parser\*.cpp parser\

xcopy /F /Y ..\..\soul-mod\soul\common\token_parser.cppm common\

xcopy /F /Y ..\..\soul-mod\soul\xml\dom\*.cppm xml\dom\
xcopy /F /Y ..\..\soul-mod\soul\xml\dom\*.cpp xml\dom\

xcopy /F /Y ..\..\soul-mod\soul\xml\dom_parser\*.cppm xml\dom_parser\
xcopy /F /Y ..\..\soul-mod\soul\xml\dom_parser\*.cpp xml\dom_parser\

xcopy /F /Y ..\..\soul-mod\soul\xml\processor\*.cppm xml\processor\
xcopy /F /Y ..\..\soul-mod\soul\xml\processor\*.cpp xml\processor\

xcopy /F /Y ..\..\soul-mod\soul\xml\xml_parser\*.cppm xml\xml_parser\
xcopy /F /Y ..\..\soul-mod\soul\xml\xml_parser\*.cpp xml\xml_parser\
xcopy /F /Y ..\..\soul-mod\soul\xml\xml_parser\*.parser xml\xml_parser\
xcopy /F /Y ..\..\soul-mod\soul\xml\xml_parser\*.spg xml\xml_parser\

xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.cppm xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.cpp xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.parser xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.expr xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.keyword xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.lexer xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.token xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.spg xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.slg xml\xpath\
xcopy /F /Y ..\..\soul-mod\soul\xml\xpath\*.bat xml\xpath\
