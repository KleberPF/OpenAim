return {
    weapon = Weapon.Machine_Gun,
    player_pos = {
        x = 0,
        y = 10,
        z = 8
    },
    challenge_duration = 30,
    targets = {
        {
            shape = Shape.Ball,
            scale = {
                x = 0.6,
                y = 0.6,
                z = 0.6
            },
            random_spawn = true,
            min_coords = {
                x = -8,
                y = 2,
                z = -8
            },
            max_coords = {
                x = 8,
                y = 18,
                z = -8
            },
            type = Type.Mover,
            health = 20,
            positioner = function(d)
                return { x = 2 * math.sin(2 * d), y = 0, z = 0 }
            end,
        },
        {
            shape = Shape.Ball,
            scale = {
                x = 0.6,
                y = 0.6,
                z = 0.6
            },
            random_spawn = true,
            min_coords = {
                x = -8,
                y = 2,
                z = -8
            },
            max_coords = {
                x = 8,
                y = 18,
                z = -8
            },
            type = Type.Mover,
            health = 20,
            positioner = function(d)
                return { x = 2 * math.sin(2 * d), y = 0, z = 0 }
            end,
        },
        {
            shape = Shape.Ball,
            scale = {
                x = 0.6,
                y = 0.6,
                z = 0.6
            },
            random_spawn = true,
            min_coords = {
                x = -8,
                y = 2,
                z = -8
            },
            max_coords = {
                x = 8,
                y = 18,
                z = -8
            },
            type = Type.Mover,
            health = 20,
            positioner = function(d)
                return { x = 2 * math.sin(2 * d), y = 0, z = 0 }
            end,
        },
        {
            shape = Shape.Ball,
            scale = {
                x = 0.6,
                y = 0.6,
                z = 0.6
            },
            random_spawn = true,
            min_coords = {
                x = -8,
                y = 2,
                z = -8
            },
            max_coords = {
                x = 8,
                y = 18,
                z = -8
            },
            type = Type.Mover,
            health = 20,
            positioner = function(d)
                return { x = 2 * math.sin(2 * d), y = 0, z = 0 }
            end,
        },
        {
            shape = Shape.Ball,
            scale = {
                x = 0.6,
                y = 0.6,
                z = 0.6
            },
            random_spawn = true,
            min_coords = {
                x = -8,
                y = 2,
                z = -8
            },
            max_coords = {
                x = 8,
                y = 18,
                z = -8
            },
            type = Type.Mover,
            health = 20,
            positioner = function(d)
                return { x = 2 * math.sin(2 * d), y = 0, z = 0 }
            end,
        },
    }
}