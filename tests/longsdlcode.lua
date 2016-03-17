--======================================================================
--  ALIENS: A silly little game demonstrating the SDL and mixer libraries
--  Copyright (C) 1998  Sam Lantinga
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--
--  Sam Lantinga
--  5635-34 Springhouse Dr.
--  Pleasanton, CA 94588 (USA)
--  slouken@devolution.com
--======================================================================
-- NOTES:
-- * Ported to LuaSDL 0.3.0 by Kein-Hong Man <khman@users.sf.net> 2007
--   See README for more information.
-- * If anything fails, please look for stdout.txt and stderr.txt.
--======================================================================

--======================================================================
-- settings
--======================================================================

local FULLSCREEN = false--true

--======================================================================
-- constants
--======================================================================

local FRAMES_PER_SEC = 50
local PLAYER_SPEED   = 4
local MAX_SHOTS	     = 3
local SHOT_SPEED     = 6
local MAX_ALIENS     = 30
local ALIEN_SPEED    = 5
local ALIEN_ODDS     = 1 * FRAMES_PER_SEC
local EXPLODE_TIME   = 4

--======================================================================
-- functions for closing, error reporting, finding data
--======================================================================

local function stderr(msg) io.stderr:write(msg) end

local function exit() SDL.SDL_Quit() end

local function DATAFILE(X)
  return SDL.LuaSDL_DirApp.."data"..SDL.LuaSDL_DirSep..X
end

--======================================================================
-- global data
--======================================================================

local screen, background

-- object: alive, facing, x, y, image
local player = {}
local reloading

local shots = {}
for i = 1, MAX_SHOTS do shots[i] = {} end

local aliens = {}
for i = 1, MAX_ALIENS do aliens[i] = {} end

local explosions = {}
for i = 1, MAX_ALIENS+1 do explosions[i] = {} end

local MAX_UPDATES = 3 * (1 + MAX_SHOTS + MAX_ALIENS)

local numupdates

local srcupdate = {}
for i = 1, MAX_UPDATES do
  srcupdate[i] = SDL.SDL_Rect_local()
end
local dstupdate = {}
for i = 1, MAX_UPDATES do
  dstupdate[i] = SDL.SDL_Rect_local()
end

-- blit: src, srcrect, dstrect
local blits = {}
for i = 1, MAX_UPDATES do blits[i] = {} end

local music

local MUSIC_WAV = 0
local SHOT_WAV = 1
local EXPLODE_WAV = 2
local NUM_WAVES = 3

local sounds = {}

--======================================================================
-- resource management
--======================================================================

local function LoadImage(datafile, transparent)
  local image = SDL.IMG_Load(datafile)
  if not image then
    stderr("Couldn't load image "..datafile..": "..SDL.IMG_GetError().."\n")
    return
  end
  if transparent then
    -- Assuming 8-bit BMP image
    SDL.SDL_SetColorKey(image, SDL.Or(SDL.SDL_SRCCOLORKEY, SDL.SDL_RLEACCEL),
                        SDL.SDL_GetPixel(image, 0, 0))
  end
  local surface = SDL_DisplayFormat(image)
  SDL.SDL_FreeSurface(image)
end

local function LoadData()
  -- Load sounds
  music = SDL.Mix_LoadMUS(DATAFILE("music.it"))
  if not music then
    stderr("Warning: Couldn't load music: "..SDL.Mix_GetError().."\n")
  end

  sounds[SHOT_WAV] = SDL.Mix_LoadWAV(DATAFILE("shot.wav"))
  sounds[EXPLODE_WAV] = SDL.Mix_LoadWAV(DATAFILE("explode.wav"))

  -- Load graphics
  player.image = LoadImage(DATAFILE("player.gif"), true)
  if not player.image then
    return false
  end
  shots[1].image = LoadImage(DATAFILE("shot.gif"))
  if not shots[1].image then
    return false
  end
  for i = 2, MAX_SHOTS do
    shots[i].image = shots[1].image
  end
  aliens[1].image = LoadImage(DATAFILE("alien.gif"), true)
  if not aliens[1].image then
    return false
  end
  for i = 2, MAX_ALIENS do
    aliens[i].image = aliens[1].image
  end
  explosions[1].image = LoadImage(DATAFILE("explosion.gif"), true)
  for i = 2, MAX_ALIENS+1 do
    explosions[i].image = explosions[1].image
  end
  background = LoadImage(DATAFILE("background.gif"))

  -- Set up the update rectangle pointers
  for i = 1, MAX_UPDATES do
    blits[i].srcrect = srcupdate[i]
    blits[i].dstrect = dstupdate[i]
  end
  return true
end

local function FreeData()
  -- Free sounds
  SDL.Mix_FreeMusic(music)
  for i = 0, NUM_WAVES do
    SDL.Mix_FreeChunk(sounds[i])
  end

  -- Free graphics
  SDL.SDL_FreeSurface(player.image)
  SDL.SDL_FreeSurface(shots[1].image)
  SDL.SDL_FreeSurface(aliens[1].image)
  SDL.SDL_FreeSurface(explosions[1].image)
  SDL.SDL_FreeSurface(background)
end

--======================================================================
-- alien spawning function
--======================================================================

local function CreateAlien()
  -- Look for a free alien slot
  local i
  for I = 1, MAX_ALIENS do
    if not aliens[I].alive then
      i = I; break
    end
  end
  if not i then return end

  -- Figure out which direction it travels
  repeat
    aliens[i].facing = math.random(3) - 2
  until aliens[i].facing ~= 0

  -- Figure out it's initial location
  aliens[i].y = 0
  if aliens[i].facing < 0 then
    aliens[i].x = screen.w - aliens[i].image.w - 1
  else
    aliens[i].x = 0
  end
  aliens[i].alive = true
end

--======================================================================
-- screen updating
--======================================================================

local function DrawObject(sprite)
  local update = blits[numupdates]
  numupdates = numupdates + 1
  update.src = sprite.image
  update.srcrect.x = 0
  update.srcrect.y = 0
  update.srcrect.w = sprite.image.w
  update.srcrect.h = sprite.image.h
  update.dstrect.x = sprite.x
  update.dstrect.y = sprite.y
  update.dstrect.w = sprite.image.w
  update.dstrect.h = sprite.image.h
end

local function EraseObject(sprite)
  -- The background wraps horizontally across the screen
  local update = blits[numupdates]
  numupdates = numupdates + 1
  update.src = background
  update.srcrect.x = sprite.x % background.w
  update.srcrect.y = sprite.y
  update.srcrect.w = sprite.image.w
  update.srcrect.h = sprite.image.h
  local wrap = (update.srcrect.x + update.srcrect.w) - (background.w)
  if wrap > 0 then
    update.srcrect.w = update.srcrect.w - wrap
  end
  update.dstrect.x = sprite.x
  update.dstrect.y = sprite.y
  update.dstrect.w = update.srcrect.w
  update.dstrect.h = update.srcrect.h

  -- Assuming sprites can only wrap across one background tile
  if wrap > 0 then
    update = blits[numupdates]
    numupdates = numupdates + 1
    update.src = background
    update.srcrect.x = 0
    update.srcrect.y = sprite.y
    update.srcrect.w = wrap
    update.srcrect.h = sprite.image.h
    update.dstrect.x = (math.floor(sprite.x / background.w) + 1) * background.w
    update.dstrect.y = sprite.y
    update.dstrect.w = update.srcrect.w
    update.dstrect.h = update.srcrect.h
  end
end

local function UpdateScreen()
  for i = 1, numupdates-1 do
    SDL.SDL_LowerBlit(blits[i].src, blits[i].srcrect, screen, blits[i].dstrect)
  end
  SDL.SDL_UpdateRects(screen, numupdates-1, dstupdate)
  numupdates = 1
end

--======================================================================
-- support functions for game
--======================================================================

local function Collide(sprite1, sprite2)
  if sprite1.y >= (sprite2.y + sprite2.image.h) or
     sprite1.x >= (sprite2.x + sprite2.image.w) or
     sprite2.y >= (sprite1.y + sprite1.image.h) or
     sprite2.x >= (sprite1.x + sprite1.image.w) then
    return false
  end
  return true
end

local next_tick = 0
local function WaitFrame()
  -- Wait for the next frame
  local this_tick = SDL.SDL_GetTicks()
  if this_tick < next_tick then
    SDL.SDL_Delay(next_tick - this_tick)
  end
  next_tick = this_tick + (1000 / FRAMES_PER_SEC)
end

--======================================================================
-- main game function
--======================================================================

-- This of course can be optimized :-)
local function RunGame()
  local event = SDL.SDL_Event_local()

  -- Paint the background
  numupdates = 1
  local dst = SDL.SDL_Rect_local()
  local i = 0
  while i < screen.w do
    dst.x = i
    dst.y = 0
    dst.w = background.w
    dst.h = background.h
    SDL.SDL_BlitSurface(background, nil, screen, dst)
    i = i + background.w
  end
  SDL.SDL_UpdateRect(screen, 0, 0, 0, 0)

  -- Initialize the objects
  player.alive = true
  player.x = (screen.w - player.image.w) / 2
  player.y = (screen.h - player.image.h) - 1
  player.facing = 0
  DrawObject(player)

  for i = 1, MAX_SHOTS do
    shots[i].alive = false
  end
  for i = 1, MAX_ALIENS do
    aliens[i].alive = false
  end
  CreateAlien()
  DrawObject(aliens[1])
  UpdateScreen()

  while player.alive do
    -- Wait for the next frame
    WaitFrame()

    -- Poll input queue, run keyboard loop
    while SDL.Bool(SDL.SDL_PollEvent(event)) do
      if event.type == SDL.SDL_QUIT then
        return
      end
    end
    local keys = SDL.SDL_GetKeyState()

    -- Erase everything from the screen
    for i = 1, MAX_SHOTS do
      if shots[i].alive then EraseObject(shots[i]) end
    end
    for i = 1, MAX_ALIENS do
      if aliens[i].alive then EraseObject(aliens[i]) end
    end
    EraseObject(player)
    for i = 1, MAX_ALIENS+1 do
      if explosions[i].alive then EraseObject(explosions[i]) end
    end

    -- Decrement the lifetime of the explosions
    for i = 1, MAX_ALIENS+1 do
      if explosions[i].alive then
        explosions[i].alive = explosions[i].alive - 1
        if explosions[i].alive == 0 then
          explosions[i].alive = nil
        end
      end
    end

    -- Create new aliens
    if math.random(ALIEN_ODDS) == 1 then
      CreateAlien()
    end

    -- Create new shots
    if not reloading then
      if keys[SDL.SDLK_SPACE] == SDL.SDL_PRESSED then
        local i
        for I = 1, MAX_SHOTS do
          if not shots[I].alive then
            i = I; break
          end
        end
        if i then
          shots[i].x = player.x + (player.image.w - shots[i].image.w) / 2
          shots[i].y = player.y - shots[i].image.h
          shots[i].alive = true
          SDL.Mix_PlayChannel(SHOT_WAV, sounds[SHOT_WAV], 0)
        end
      end
    end
    reloading = keys[SDL.SDLK_SPACE] == SDL.SDL_PRESSED

    -- Move the player
    player.facing = 0
    if keys[SDL.SDLK_RIGHT] ~= 0 then
      player.facing = player.facing + 1
    end
    if keys[SDL.SDLK_LEFT] ~= 0 then
      player.facing = player.facing - 1
    end
    player.x = player.x + player.facing * PLAYER_SPEED
    if player.x < 0 then
      player.x = 0
    elseif player.x >= (screen.w - player.image.w) then
      player.x = (screen.w - player.image.w) - 1
    end

    -- Move the aliens
    for i = 1, MAX_ALIENS do
      if aliens[i].alive then
        aliens[i].x = aliens[i].x + aliens[i].facing * ALIEN_SPEED
        if aliens[i].x < 0 then
          aliens[i].x = 0
          aliens[i].y = aliens[i].y + aliens[i].image.h
          aliens[i].facing = 1
        elseif aliens[i].x >= (screen.w - aliens[i].image.w) then
          aliens[i].x = (screen.w - aliens[i].image.w) - 1
          aliens[i].y = aliens[i].y + aliens[i].image.h
          aliens[i].facing = -1
        end
      end
    end

    -- Move the shots
    for i = 1, MAX_SHOTS do
      if shots[i].alive then
        shots[i].y = shots[i].y - SHOT_SPEED
        if shots[i].y < 0 then
          shots[i].alive = false
        end
      end
    end

    -- Detect collisions
    for j = 1, MAX_SHOTS do
      for i = 1, MAX_ALIENS do
        if shots[j].alive and aliens[i].alive and Collide(shots[j], aliens[i]) then
          aliens[i].alive = false
          explosions[i].x = aliens[i].x
          explosions[i].y = aliens[i].y
          explosions[i].alive = EXPLODE_TIME
          SDL.Mix_PlayChannel(EXPLODE_WAV, sounds[EXPLODE_WAV], 0)
          shots[j].alive = false
          break
        end
      end
    end
    for i = 1, MAX_ALIENS do
      if aliens[i].alive and Collide(player, aliens[i]) then
        aliens[i].alive = false
        explosions[i].x = aliens[i].x
        explosions[i].y = aliens[i].y
        explosions[i].alive = EXPLODE_TIME
        player.alive = false
        explosions[MAX_ALIENS].x = player.x
        explosions[MAX_ALIENS].y = player.y
        explosions[MAX_ALIENS].alive = EXPLODE_TIME
        SDL.Mix_PlayChannel(EXPLODE_WAV, sounds[EXPLODE_WAV], 0)
      end
    end

    -- Draw the aliens, shots, player, and explosions
    for i = 1, MAX_ALIENS do
      if aliens[i].alive then DrawObject(aliens[i]) end
    end
    for i = 1, MAX_SHOTS do
      if shots[i].alive then DrawObject(shots[i]) end
    end
    if player.alive then DrawObject(player) end
    for i = 1, MAX_ALIENS+1 do
      if explosions[i].alive then DrawObject(explosions[i]) end
    end
    UpdateScreen()

    -- Loop the music
    if not SDL.Bool(SDL.Mix_PlayingMusic()) then
      SDL.Mix_PlayMusic(music, 0)
    end

    -- Check for keyboard abort
    if keys[SDL.SDLK_ESCAPE] == SDL.SDL_PRESSED then
      player.alive = false
    end
  end--while

  -- Wait for the player to finish exploding
  while SDL.Bool(SDL.Mix_Playing(EXPLODE_WAV)) do
    WaitFrame()
  end

  SDL.Mix_HaltChannel(-1)
end

--======================================================================
-- main program starts here
--======================================================================

-- Initialize the SDL library
if SDL.SDL_Init(SDL.Or(SDL.SDL_INIT_AUDIO, SDL.SDL_INIT_VIDEO)) < 0 then
  stderr("Couldn't initialize SDL: "..SDL.SDL_GetError().."\n")
  return
end

-- Open the audio device
if SDL.Mix_OpenAudio(11025, SDL.AUDIO_U8, 1, 512) < 0 then
  stderr("Warning: Couldn't set 11025 Hz 8-bit audio\n- Reason: "..SDL.SDL_GetError().."\n")
  exit()
end

-- Open the display device
local flags = SDL.SDL_SWSURFACE
if FULLSCREEN then
  flags = SDL.Or(flags, SDL.SDL_FULLSCREEN)
end

screen = SDL.SDL_SetVideoMode(640, 480, 0, flags)
if not screen then
  stderr("Couldn't set 640x480 video mode: "..SDL.SDL_GetError().."\n")
  exit()
end

-- Initialize the random number generator
math.randomseed(os.time())

-- Load the music and artwork
if LoadData() then
  -- Run the game
  RunGame()
  -- Free the music and artwork
  FreeData()
end

-- Quit
SDL.Mix_CloseAudio()
exit()
