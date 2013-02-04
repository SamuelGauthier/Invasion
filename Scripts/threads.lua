info = {
	name = "Threads",
	group = "Threads",
	depedency = "",
	entry_point = "",
}

co_create = coroutine.create
co_running = coroutine.running
co_resume = coroutine.resume
co_yield = coroutine.yield

local bedroom = {}
activeThread = nil 	-- only one running thread at a time
local threads = {}
local yielded = false

----------------------------------------------------------------------
-- Threads
----------------------------------------------------------------------
function StartThread(fun, bg,...)
	local co = co_create(fun)

	local thread = {
		thread = co,
		id = #threads+1,
		background = bg,
		unitid = activeUnitID,
	}

	local oldThread = activeThread
	activeThread = thread
	threads[thread.id] = thread

	co_resume(co,...)

	activeThread = oldThread
	return thread
end

function StandBy()
	bedroom[activeThread.id] = activeThread
	activeThread = nil
 
 	yielded = true
	co_yield()
end

function Sleep(t)
	SleepA(t)
	StandBy()
end

function WaitForMove(t)
	WaitForMoveA(t)
	StandBy()
end

function SelectPlace(t)
	SelectPlaceA(t)
	StandBy()
end

function WaitForKey()
	WaitForKeyA()
	StandBy()
end

-- waky waky...
function WakeUp(index)
	if bedroom[index] then
		activeThread = bedroom[index]
		bedroom[index] = nil

		local good, msg = co_resume(activeThread.thread)
		if not yielded and not activeThread.background then
			KillTasksWithThread(activeThread.id)
		else
			yielded = false
		end
		if not good then print(msg) end
	end
end
