using System;
using System.Collections.Generic;
using System.Linq;

namespace ANT_Console.Services
{
    public static class Statistics
    {
        public static double Mean(this IEnumerable<double> list)
        {
            return list.Average(); // :-)
        }
 
        public static double Median(this IEnumerable<double> list)
        {
            List<double> orderedList = list
                .OrderBy(numbers => numbers)
                .ToList();
 
            int listSize = orderedList.Count;
            double result;
 
            if (listSize%2 == 0) // even
            {
                int midIndex = listSize/2;
                result = ((orderedList.ElementAt(midIndex - 1) +
                           orderedList.ElementAt(midIndex))/2);
            }
            else // odd
            {
                double element = (double) listSize/2;
                element = Math.Round(element, MidpointRounding.AwayFromZero);
 
                result = orderedList.ElementAt((int) (element - 1));
            }
 
            return result;
        }
 
        public static IEnumerable<double> Modes(this IEnumerable<double> list)
        {
            var modesList = list
                .GroupBy(values => values)
                .Select(valueCluster =>
                        new
                            {
                                Value = valueCluster.Key,
                                Occurrence = valueCluster.Count(),
                            })
                .ToList();
 
            int maxOccurrence = modesList
                .Max(g => g.Occurrence);
 
            return modesList
                .Where(x => x.Occurrence == maxOccurrence && maxOccurrence > 1) // Thanks Rui!
                .Select(x => x.Value);
        }
    }
}
