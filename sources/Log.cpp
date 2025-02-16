#include "imgui/imgui.h"

#include <cstdarg> // va_list(), va_start(), va_end()
#include <cstdio> // fprintf(), FILE
#include <string_view> // std::string_view{}

#include "Log.hpp" // TR_LOG()

class Log {
public:
  enum class Level { ERROR, WARNING, DEBUG, INFO };
  static std::string_view LevelName(Level level) NOEXCEPT;

public:
  Log(void) NOEXCEPT;

  void Clear(void) NOEXCEPT;
  void Write(Level level, const char* format, va_list args) NOEXCEPT TR_FMTLIST(3);

  void RenderLine(char const* start, char const* end) NOEXCEPT;
  void Render(char const* title) NOEXCEPT;

private:
  bool m_autoScroll = true;
  ImGuiTextBuffer m_buffer;
  ImGuiTextFilter m_filter;
  ImVector<int> m_lineOffsets;
};

static Log s_log;

void GlobalLogRender(char const* title) NOEXCEPT {
  s_log.Render(title);
}

void GlobalLog(FILE* stream, char const* format, ...) NOEXCEPT {
  va_list args;

  va_start(args, format);
  vfprintf(stream, format, args);
  va_end(args);

  Log::Level level = stream == stderr ? Log::Level::ERROR : Log::Level::INFO;

  va_start(args, format);
  s_log.Write(level, format, args);
  va_end(args);
}

std::string_view Log::LevelName(Level level) NOEXCEPT {
  switch (level) {
    case Level::ERROR:   return "Error";
    case Level::WARNING: return "Warning";
    case Level::DEBUG:   return "Debug";
    case Level::INFO:    return "Info";
    default:             return "??";
  }
}

Log::Log(void) NOEXCEPT {
  Clear();
}

void Log::Clear(void) NOEXCEPT {
  m_buffer.clear();
  m_lineOffsets.clear();
  m_lineOffsets.push_back(0);
}

void Log::Write(Log::Level level, const char* format, va_list args) NOEXCEPT {
  int oldSize = m_buffer.size();
  // TODO: Add time before log level.
  std::string_view name = LevelName(level);
  int nameSize = static_cast<int>(name.size());
  m_buffer.appendf("[%*.*s] ", nameSize, nameSize, name.data());
  m_buffer.appendfv(format, args);
  for (int newSize = m_buffer.size(); oldSize < newSize; ++oldSize) {
    if (m_buffer[oldSize] == '\n') {
      m_lineOffsets.push_back(oldSize + 1);
    }
  }
}

void Log::RenderLine(char const* start, char const* end) NOEXCEPT {
  if (start[0] == '[') {
    std::string_view line(start + 1, end);
    if (line.starts_with(LevelName(Log::Level::ERROR))) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
      ImGui::TextUnformatted(start, end);
      ImGui::PopStyleColor();
      return;
    }
  }

  ImGui::TextUnformatted(start, end);
}

// Everything is taked (and improved) from ImGui Demo.
void Log::Render(char const* title) NOEXCEPT {
  if (!ImGui::Begin(title)) {
    ImGui::End();
    return;
  }

  if (ImGui::BeginPopup("Options")) {
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    ImGui::EndPopup();
  }

  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("Options");
  }

  ImGui::SameLine(); bool clear = ImGui::Button("Clear");
  ImGui::SameLine(); m_filter.Draw("Filter", 400.0f);
  ImGui::Separator();

  ImGuiChildFlags childFlags = ImGuiChildFlags_None;;
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
  if (ImGui::BeginChild("Scrolling", ImVec2(0, 0), childFlags, windowFlags)) {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    // if (copy) ImGui::LogToClipboard(); ??
    if (clear) Clear();

    char const* bufferStart = m_buffer.begin();
    char const* bufferEnd = m_buffer.end();

    if (m_filter.IsActive()) {
      for (int lineNumber = 0; lineNumber < m_lineOffsets.Size; ++lineNumber) {
        char const* lineStart = bufferStart + m_lineOffsets[lineNumber];
        const char* lineEnd = (lineNumber + 1 < m_lineOffsets.Size)
          ? (bufferStart + m_lineOffsets[lineNumber + 1] - 1)
          : bufferEnd;

        if (m_filter.PassFilter(lineStart, lineEnd)) {
          RenderLine(lineStart, lineEnd);
        }
      }
    }
    else {
      // If you have tens of thousands of items and their processing cost is
      // non-negligible, clipping them is recommended. Using ImGuiListClipper
      // requires: A) random access into your data; B) items all being the same
      // height. Otherwise, use ImGui::TextUnformatted(bufferStart, bufferEnd).
      ImGuiListClipper clipper;
      clipper.Begin(m_lineOffsets.Size);
      while (clipper.Step()) {
        for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; ++lineNumber) {
          char const* lineStart = bufferStart + m_lineOffsets[lineNumber];
          const char* lineEnd = (lineNumber + 1 < m_lineOffsets.Size)
            ? (bufferStart + m_lineOffsets[lineNumber + 1] - 1)
            : bufferEnd;

          RenderLine(lineStart, lineEnd);
        }
      }
      clipper.End();
    }

    ImGui::PopStyleVar();
    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0f);
    }
  }
  ImGui::EndChild();
  ImGui::End();
}
