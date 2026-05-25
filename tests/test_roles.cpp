#include "core/UserManager.h"

#include <gtest/gtest.h>

TEST(UserManager, StartsWithDemoAccounts) {
    UserManager users;

    ASSERT_GE(users.users().size(), 3U);
    EXPECT_EQ(users.users()[0].name, "guest");
    EXPECT_EQ(users.users()[1].name, "student");
    EXPECT_EQ(users.users()[2].name, "admin");
}

TEST(UserManager, AuthenticatesKnownUsers) {
    UserManager users;

    EXPECT_EQ(users.authenticate("guest", ""), 0);
    EXPECT_EQ(users.authenticate("student", "student"), 1);
    EXPECT_EQ(users.authenticate("admin", "admin"), 2);
}

TEST(UserManager, RejectsInvalidCredentials) {
    UserManager users;

    EXPECT_LT(users.authenticate("student", "wrong"), 0);
    EXPECT_LT(users.authenticate("missing", "student"), 0);
    EXPECT_LT(users.authenticate("", ""), 0);
}

TEST(UserManager, RegistersNewUserAccounts) {
    UserManager users;

    const int newUserIndex = users.registerUser("new_user", "New User", "secret");
    ASSERT_GE(newUserIndex, 0);

    EXPECT_EQ(users.authenticate("new_user", "secret"), newUserIndex);
    EXPECT_EQ(users.users()[static_cast<std::size_t>(newUserIndex)].role, UserRole::User);
    EXPECT_FALSE(users.users()[static_cast<std::size_t>(newUserIndex)].blocked);
}

TEST(UserManager, RejectsInvalidRegistrationInput) {
    UserManager users;

    EXPECT_LT(users.registerUser("", "No Login", "secret"), 0);
    EXPECT_LT(users.registerUser("no_password", "No Password", ""), 0);
    EXPECT_LT(users.registerUser("student", "Duplicate", "secret"), 0);
}

TEST(UserManager, UsesLoginAsDefaultDisplayName) {
    UserManager users;

    const int userIndex = users.registerUser("plain", "", "secret");

    ASSERT_GE(userIndex, 0);
    EXPECT_EQ(users.users()[static_cast<std::size_t>(userIndex)].displayName, "plain");
}

TEST(UserManager, UpdatesProfileForValidIndex) {
    UserManager users;

    const int studentIndex = users.authenticate("student", "student");
    ASSERT_GE(studentIndex, 0);

    EXPECT_TRUE(users.updateProfile(studentIndex, "Updated Student", "newpass"));
    EXPECT_LT(users.authenticate("student", "student"), 0);
    EXPECT_EQ(users.authenticate("student", "newpass"), studentIndex);
    EXPECT_EQ(users.users()[static_cast<std::size_t>(studentIndex)].displayName, "Updated Student");
}

TEST(UserManager, RejectsProfileUpdateForInvalidIndex) {
    UserManager users;

    EXPECT_FALSE(users.updateProfile(-1, "Invalid", "pass"));
    EXPECT_FALSE(users.updateProfile(100, "Invalid", "pass"));
}

TEST(UserManager, KeepsDisplayNameWhenProfileNameIsEmpty) {
    UserManager users;

    ASSERT_TRUE(users.updateProfile(1, "", "newpass"));

    EXPECT_EQ(users.users()[1].displayName, "Student");
    EXPECT_EQ(users.authenticate("student", "newpass"), 1);
}

TEST(UserManager, EnforcesCreateAndSavePermissions) {
    EXPECT_FALSE(UserManager::canCreateMaze(UserRole::Guest, false));
    EXPECT_TRUE(UserManager::canCreateMaze(UserRole::User, false));
    EXPECT_TRUE(UserManager::canCreateMaze(UserRole::Admin, false));
    EXPECT_FALSE(UserManager::canCreateMaze(UserRole::User, true));

    EXPECT_FALSE(UserManager::canSaveMaze(UserRole::Guest, false));
    EXPECT_TRUE(UserManager::canSaveMaze(UserRole::User, false));
    EXPECT_FALSE(UserManager::canSaveMaze(UserRole::Admin, true));
}

TEST(UserManager, EnforcesModerationPermissions) {
    EXPECT_FALSE(UserManager::canModerate(UserRole::Guest, false));
    EXPECT_FALSE(UserManager::canModerate(UserRole::User, false));
    EXPECT_TRUE(UserManager::canModerate(UserRole::Admin, false));
    EXPECT_FALSE(UserManager::canModerate(UserRole::Admin, true));
}

TEST(UserManager, BlocksAndUnblocksNonAdminAccounts) {
    UserManager users;

    EXPECT_TRUE(users.setBlocked("student", true));
    EXPECT_TRUE(users.users()[1].blocked);
    EXPECT_TRUE(users.setBlocked("student", false));
    EXPECT_FALSE(users.users()[1].blocked);
}

TEST(UserManager, RejectsBlockingAdminAndUnknownUsers) {
    UserManager users;

    EXPECT_FALSE(users.setBlocked("admin", true));
    EXPECT_FALSE(users.setBlocked("missing", true));
}
