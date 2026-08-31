return {
    name = "Tracking",
    weapon = Weapon.Machine_Gun,
    player_pos = {
        x = 0,
        y = 1.5,
        z = 8
    },
    challenge_duration = 60,
    win_condition = WinCondition.ClearTargets,
    targets = {
        {
            shape = Shape.Box,
            scale = {
                x = 0.3,
                y = 3,
                z = 0.3
            },
            spawn_coords = {
                x = 0,
                y = 1.55,
                z = -8
            },
            type = Type.Goner,
            health = 200,
            positioner = function(ref, cur, d)
                return { x = ref.x + 5 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        }
    }
}
