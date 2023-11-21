// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <Windows.h>

import springpp.main_window;
import springpp.theme;
import springpp.layout;
import springpp.configuration;
import springpp.primitive;
import wing;
import util;
import std.core;
import std.filesystem;

void InitApplication(HINSTANCE instance)
{
    util::Init();
    wing::Init(instance);
    springpp::InitThemes();
}

void TerminateApplication()
{
    wing::Done();
    util::Done();
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    try
    {
        wing::PointF p(0, 0);
        wing::PointF q(30, 10);
        springpp::Line pq(p, q);
        wing::PointF a(1, 0);
        springpp::Line pa(p, a);
        springpp::Vector u = pq.ToVector();
        springpp::Vector unit = pa.ToVector();
        springpp::Vector v = Projection(u, unit);
        springpp::Vector w = u - v;
        float sinAlfa = w.Length() / u.Length();
        float cosAlfa = v.Length() / u.Length();
        float h = 4.0f;
        float y = h - h * cosAlfa * sinAlfa;
        float x = 0.0f;
        if (sinAlfa)
        {
            x = y * cosAlfa / sinAlfa;
        }
        InitApplication(instance);
        springpp::MainWindow mainWindow;
        wing::Graphics* graphics = wing::Graphics::FromHWND(mainWindow.Handle());
        std::unique_ptr<springpp::Layout> layout(new springpp::Layout(graphics, "layout.xml"));
        if (!std::filesystem::exists(layout->XmlFilePath()))
        {
            layout->Save();
        }
        layout->Load();
        springpp::Configuration::Instance().SetLayout(layout.release());
        mainWindow.SetSmallIcon(wing::Application::GetResourceManager().GetIcon("spring.icon"));
        mainWindow.SetIcon(wing::Application::GetResourceManager().GetIcon("spring.icon"));
        int exitCode = wing::Application::Run(mainWindow);
        return exitCode;
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    TerminateApplication();
    return 0;
}
