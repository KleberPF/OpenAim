local function create_strafer(speed)
    local target
    local last_time = 0

    return function(ref, cur, d)
        if last_time == 0 then
            target = {
                x = cur.x,
                y = cur.y,
                z = cur.z,
            }
        end

        local dt = d - last_time
        last_time = d

        local offset = {
            x = target.x - cur.x,
            y = target.y - cur.y,
            z = target.z - cur.z,
        }

        dist = math.sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z)
        if dist < 0.2 then
            -- Arrived at the target, generate a new one
            target = {
                x = ref.x + math.random() - 0.5,
                y = ref.y + math.random() - 0.5,
                z = -8,
            }
        end

        function norm(n)
            if n == 0 then return 0
            elseif n < 0 then return -1
            else return 1 end
        end 

        local norm_offset = {
            x = norm(offset.x),
            y = norm(offset.y),
            z = norm(offset.z),
        }

        local current_target = {
            x = cur.x + norm_offset.x * dt * speed,
            y = cur.y + norm_offset.y * dt * speed,
            z = cur.z + norm_offset.z * dt * speed
        }

        return current_target
    end
end

return {
    name = "Tracking Ball",
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
            shape = Shape.Ball,
            scale = 0.3,
            spawn_coords = {
                x = 0,
                y = 1.55,
                z = -8
            },
            destroyable = false,
            positioner = create_strafer(0.8),
        }
    }
}
