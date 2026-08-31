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
            scale = 0.6,
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
            positioner = function(ref, cur, d)
                return { x = ref.x + 2 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        },
        {
            shape = Shape.Ball,
            scale = 0.6,
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
            positioner = function(ref, cur, d)
                return { x = ref.x + 2 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        },
        {
            shape = Shape.Ball,
            scale = 0.6,
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
            positioner = function(ref, cur, d)
                return { x = ref.x + 2 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        },
        {
            shape = Shape.Ball,
            scale = 0.6,
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
            positioner = function(ref, cur, d)
                return { x = ref.x + 2 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        },
        {
            shape = Shape.Ball,
            scale = 0.6,
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
            positioner = function(ref, cur, d)
                return { x = ref.x + 2 * math.sin(2 * d), y = cur.y, z = cur.z }
            end,
        },
    }
}