# This class generates sweeping value within the specified range.
# Sweeping begins from minValue to maxValue in the configured sweepTime.
# The peak value is held for a set duration (constantTime).
# Then values sweep from maxValue to minValue and the minValue  is held for a set duration

class sweeper(object):
   (Increasing, Peak, Decreasing, Valley) = range(4)
   minValue = 10
   maxValue = 60
   sweepTime = 60
   constantTime = 5 

   def start(self):
      self.__newValue = self.minValue
      self.__slope = (self.maxValue - self.minValue)/self.sweepTime
      self.__previousTime = 0
      self.__stateCounter = 0
      self.__state = self.Increasing
      from System.Diagnostics import Stopwatch
      self.__stopWatch = Stopwatch()
      self.__stopWatch.Start()

   def stop(self):
      self.__stopWatch.Stop()

   def getNextValue(self):
      currentTime = self.__stopWatch.ElapsedMilliseconds
      elapsedTime = (currentTime - self.__previousTime)/1000
      self.__previousTime = currentTime
      if self.__state == self.Increasing:
         self.__stateCounter += elapsedTime
         self.__newValue += self.__slope * elapsedTime
         if self.__stateCounter >= self.sweepTime:
            self.__newValue = self.maxValue
            self.__stateCounter = 0
            self.__state = self.Peak
      elif self.__state == self.Peak:
            self.__stateCounter += elapsedTime
            if self.__stateCounter >= self.constantTime:
               self.__stateCounter = 0
               self.__state = self.Decreasing
      elif self.__state == self.Decreasing:
         self.__stateCounter += elapsedTime
         self.__newValue -= self.__slope * elapsedTime
         if self.__stateCounter >= self.sweepTime:
            self.__newValue = self.minValue
            self.__stateCounter = 0
            self.__state = self.Valley
      elif self.__state == self.Valley:
            self.__stateCounter += elapsedTime
            if self.__stateCounter >= self.constantTime:
               self.__stateCounter = 0
               self.__state = self.Increasing
      return self.__newValue
