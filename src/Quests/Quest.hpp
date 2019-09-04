#pragma once

#include <Urho3D/Core/Object.h>

#include <vector>

class Quest : public Urho3D::Object {
    URHO3D_OBJECT(Quest, Object);

public:
    explicit Quest(Urho3D::Context* context, bool is_available = true);

    [[nodiscard]] virtual bool is_finished() const = 0;
    ~Quest() override = default;

    [[nodiscard]] const Urho3D::String& get_name() const;
    [[nodiscard]] virtual Urho3D::String get_description() const;
    void move_page(int offset);
    [[nodiscard]] bool at_last_page() const;

protected:
    enum stage {
        unavailable = -1,
        available = 0,
        in_progress,
        done,
        failed,
        timed_out,
    };
    stage m_current_stage;
    Urho3D::String m_name = "__MISSING_QUEST_NAME__";
    std::vector<Urho3D::String> m_pages;
    int m_current_page = 0;
};

static auto first_quest_page_1 = \
"Witaj podrozniku!\n"
"Czeka cie niesamowita przygoda!\n"
"Czy chcesz sie przekonac jaka?";

static auto first_quest_page_2 = \
"A wiec jestes zainteresowany?\n"
"Coz... nie pozostaje mi nic jak przedstawic ci zadanie.\n"
"Mozesz za nie otrzymac 100$!";

inline Urho3D::String wrapped_text(const char* s, unsigned width = 25)
{
    Urho3D::String result;
    unsigned line_width = 0;
    while (s[0] != '\0') {
        result += s[0];
        if (s[0] == '\n') {
            line_width = 0;
        }
        if (++line_width > width && s[0] != '\n' && s[1] != '\n') {
            result += '\n';
            line_width = 0;
        }
        ++s;
    }
    return result;
}

class FirstQuest : public Quest {
public:
    explicit FirstQuest(Urho3D::Context* context) : Quest(context)
    {
        m_name = "First Quest";
        m_pages.emplace_back(wrapped_text(first_quest_page_1));
        m_pages.emplace_back(wrapped_text(first_quest_page_2));
    }

    [[nodiscard]] bool is_finished() const override
    {
        return false;
    }
};