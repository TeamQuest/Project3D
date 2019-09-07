#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include "Urho3D/Core/Object.h"
#include "Urho3D/UI/Button.h"
#pragma clang diagnostic pop

#include <vector>

class QuestRunner;

class Quest : public Urho3D::Object {
    URHO3D_OBJECT(Quest, Object);

public:
    explicit Quest(Urho3D::Context* context, bool is_available = true);

    virtual bool is_finished() = 0;
    ~Quest() override = default;

    [[nodiscard]] const Urho3D::String& get_name() const;
    [[nodiscard]] virtual Urho3D::String get_description() const;
    [[nodiscard]] bool at_last_page() const;
    void move_page(int offset);
    void assign_to(QuestRunner* runner);
    void unassign(bool set_available = true);

    friend void set_button_based_on_stage(Quest* quest, Urho3D::Button* button);

protected:
    enum stage {
        unavailable = -1,
        available = 0,
        in_progress,
        completed,
        done,
        failed,
        timed_out,
    };
    stage m_current_stage;
    Urho3D::String m_name = "__MISSING_QUEST_NAME__";
    std::vector<Urho3D::String> m_pages;
    int m_current_page = 0;
    Urho3D::WeakPtr<QuestRunner> m_runner;
};

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

        static auto first_quest_page_1 = (
        "Witaj podrozniku!\n"
        "Czeka cie niesamowita przygoda!\n"
        "Czy chcesz sie przekonac jaka?");

        static auto first_quest_page_2 = (
        "A wiec jestes zainteresowany?\n"
        "Coz... nie pozostaje mi nic jak przedstawic ci zadanie.\n"
        "Mozesz za nie otrzymac 100$!");

        m_name = "First Quest";
        m_pages.emplace_back(wrapped_text(first_quest_page_1));
        m_pages.emplace_back(wrapped_text(first_quest_page_2));
    }

    bool is_finished() override { return false; }
};

class SecondQuest : public Quest {
public:
    explicit SecondQuest(Urho3D::Context* context) : Quest(context)
    {
        m_name = "Second Quest";
        m_pages.emplace_back(wrapped_text("PAGE_1_MISSING"));
        m_pages.emplace_back(wrapped_text("PAGE_2_MISSING"));
        m_pages.emplace_back(wrapped_text("PAGE_3_MISSING"));
    }

    bool is_finished() override { return false; }
};

class KillEnemy : public Quest {
public:
    explicit KillEnemy(Urho3D::Context* context) : Quest(context)
    {
        static auto page_1 = (
                "W okolicy grasuje Joe the Killer\n"
                "Musisz go zabic. ");
        static auto page_2 = ("Widze w Tobie zapal!\n"
                              "Podejmiesz sie tego zadania?");
        m_name = "Kill Joe the Killer";
        m_pages.emplace_back(wrapped_text(page_1));
        m_pages.emplace_back(wrapped_text(page_2));
    }

    bool is_finished() override
    {
        auto completed = GetGlobalVar("is_joe_killed").GetBool();
        if (completed) {
            m_current_stage = Quest::completed;
        }
        return completed;
    }
};
