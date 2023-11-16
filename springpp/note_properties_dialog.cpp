// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.note_properties_dialog;

namespace springpp {

NotePropertiesDialog::NotePropertiesDialog(NoteElement* noteElement_) : wing::Window(wing::WindowCreateParams().Defaults().Text("Note Properties").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(70)))),
    noteElement(noteElement_), cancelButton(nullptr), okButton(nullptr)
{
}

} // namespace springpp
