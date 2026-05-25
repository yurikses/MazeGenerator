# Test Scenarios

The scenario programs are small console applications built into `build/tests/scenarios`.
They exercise complete workflows on top of the same core classes used by the GUI.

## ScenarioUserGeneration

1. Register a new user.
2. Authenticate the user.
3. Generate a maze with DFS.
4. Select valid endpoints.
5. Run BFS path search.
6. Save the generated maze.

## ScenarioGuestView

1. Prepare one published and one private maze.
2. Switch to guest access.
3. Verify that only published mazes are visible.
4. Open the published maze.
5. Run path search for viewing.

## ScenarioAdminModeration

1. Create content as a normal user.
2. Switch to administrator.
3. Verify administrator sees all content.
4. Delete a maze.
5. Block and unblock a user account.

## ScenarioAlgorithmComparison

1. Generate a maze.
2. Run BFS and A* on the same maze.
3. Verify both algorithms find a path.
4. Verify both algorithms produce the same shortest path length.
