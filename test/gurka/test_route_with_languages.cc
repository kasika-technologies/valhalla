#include "gurka.h"
#include <gtest/gtest.h>

#if !defined(VALHALLA_SOURCE_DIR)
#define VALHALLA_SOURCE_DIR
#endif

using namespace valhalla;

class RouteWithStreetnameAndSignEnUs : public ::testing::Test {
protected:
  static gurka::map map;

  static void SetUpTestSuite() {
    constexpr double gridsize_metres = 100;

    const std::string ascii_map = R"(
                       J
                       |
                       |
                       |
                       I
                      /|\
                    /  |  \
                  /    |    \
           L----K-------------H----G
           A----B-------------E----F
                  \    |    /
                    \  |  /
                      \|/
                       C
                       |
                       |
                       |
                       D
               O------PM------Q
                       |
                       |
                       |
                       N

    )";

    const gurka::ways ways = {
        {"ABEF", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
        {"GHKL", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
        {"JICDMN",
         {{"highway", "primary"},
          {"name", "6th Avenue"},
          {"name:ru", "6-я авеню"},
          {"ref", "SR 37"}}},
        {"BC",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "126B"},
          {"destination", "York;Lancaster"},
          {"destination:lang:ru", "Йорк;Ланкастер"},
          {"destination:street", "6th Avenue"},
          {"destination:street:lang:ru", "6-я авеню"},
          {"destination:ref", "SR 37"}}},
        {"CE",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination:ref", "I 70 East"},
          {"destination:ref:lang:ru", "Я 70 Восток"}}},
        {"HI",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "126B"},
          {"destination:street:to", "Main Street"},
          {"destination:street:to:lang:ru", "Главная улица"},
          {"destination:ref:to", "I 80"},
          {"destination:ref:to:lang:ru", "Я 80"}}},
        {"IK",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination:ref", "I 70 West"},
          {"destination:ref:lang:ru", "Я 70 Запад"}}},
        {"OPMQ",
         {{"highway", "secondary"}, {"name", "West 8th Street"}, {"name:ru", "Западная 8-я стрит"}}},
        {"DP",
         {{"highway", "secondary_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination", "York"},
          {"destination:street", "West 8th Street"},
          {"destination:street:lang:ru", "Западная 8-я стрит"}}},
    };

    const gurka::nodes nodes = {{"M", {{"highway", "traffic_signals"}, {"name", "M Junction"}}}};

    const auto layout =
        gurka::detail::map_to_coordinates(ascii_map, gridsize_metres, {-82.68811, 40.22535});
    // TODO: determine the final name for language_admin.sqlite
    map = gurka::buildtiles(layout, ways, nodes, {},
                            "test/data/gurka_route_with_streetname_and_sign_en_us",
                            {{"mjolnir.admin",
                              {VALHALLA_SOURCE_DIR "test/data/language_admin.sqlite"}}});
  }
};

gurka::map RouteWithStreetnameAndSignEnUs::map = {};

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSignEnUs, CheckStreetNamesAndSigns1) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"A", "D"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "6th Avenue/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(0)
                .text(),
            "SR 37");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .text(),
            "6th Avenue");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "York");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Lancaster");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  // Verify street name language tag is en
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSignEnUs, CheckStreetNamesAndSigns2) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"G", "J"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "6th Avenue/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "I 80");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Main Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSignEnUs, CheckGuideSigns) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "O"}, "auto");
  gurka::assert::raw::expect_path(result, {"6th Avenue/SR 37", "6th Avenue/SR 37", "6th Avenue/SR 37",
                                           "", "West 8th Street"});

  // Verify starting on 6th Avenue/SR 37
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(01).value(),
            "SR 37");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .text(),
            "West 8th Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .text(),
            "York");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSignEnUs, CheckJunctionName) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "Q"}, "auto");
  gurka::assert::raw::expect_path(result, {"6th Avenue/SR 37", "6th Avenue/SR 37", "6th Avenue/SR 37",
                                           "6th Avenue/SR 37", "West 8th Street"});

  // Verify starting on 6th Avenue/SR 37
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  /*
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names(0)
                .text(),
            "M Junction");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  // Verify junction name pronunciation instructions
  gurka::assert::raw::expect_instructions_at_maneuver_index(
      result, maneuver_index, "Turn left at M Junction.",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Continue for 400 meters.");
*/
}
