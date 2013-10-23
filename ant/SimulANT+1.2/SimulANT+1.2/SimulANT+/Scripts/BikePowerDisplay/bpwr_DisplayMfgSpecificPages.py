# This scripts implements and decodes a received manufacturer specific page

# Implement a manufacturer specific page by extending DataPage
# Add a reference to the SimulANT+ assembly so we can use the public classes in this assembly
import clr
clr.AddReference('SimulANT+')
from AntPlus.Profiles.Components import DataPage

class sampleMfgPage(DataPage):
   fieldOne = 50 # one byte
   fieldTwo = 31 # one byte
   number = 245

   def get_DataPageNumber(self):
      return self.number

   def Encode(self, txBuffer):
      txBuffer[0] = self.number
      txBuffer[1] = self.fieldOne
      txBuffer[2] = self.fieldTwo
      txBuffer[3] = 255
      txBuffer[4] = 255
      txBuffer[5] = 255
      txBuffer[6] = 255
      txBuffer[7] = 255

   def Decode(self, rxBuffer):
      self.fieldOne = rxBuffer[1]
      self.fieldTwo = rxBuffer[2]


def ManufacturerSpecificPageReceived(page, count):
   if page.DataPageNumber == 245:
      mfgPage = sampleMfgPage()
      DataPage.Copy(page, mfgPage)
      string = 'Mfg Specific Field One: %d' %(mfgPage.fieldOne)
      logScriptEvent(string)
      # Alternatively, you could decode the received page directly, wthout creating an object, e.g.
      # fieldOne = page.RawContent[1]


def stopScript():
   simulator.TurnOff()


simulator.TurnOn()