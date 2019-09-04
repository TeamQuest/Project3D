#include "Quests/Quest.hpp"

using namespace Urho3D;

Quest::Quest(Context* context, bool is_available)
    : Object(context),
      m_current_stage{is_available ? stage::available : stage::unavailable}
{
}

[[nodiscard]] const String& Quest::get_name() const
{
    return m_name;
}

[[nodiscard]] String Quest::get_description() const
{
    if (m_current_page < m_pages.size()) {
        return m_pages[m_current_page];
    }
    else {
        return "__MISSING_PAGE__";
    }
}

void Quest::move_page(int offset) {
    m_current_page = Clamp(m_current_page + offset, 0, static_cast<int>(m_pages.size()) - 1);
}

bool Quest::at_last_page() const {
    return m_current_page == m_pages.size() - 1;
}
