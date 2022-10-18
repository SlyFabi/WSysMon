#import "MacUtils.h"

#import <Foundation/Foundation.h>
#import <cairo-quartz.h>

std::vector<int> MacUtils::GetAllPidsWithWindows() {
    NSArray *windowList = (id)CGWindowListCopyWindowInfo(kCGWindowListOptionAll
                                                         | kCGWindowListExcludeDesktopElements,
                                                         kCGNullWindowID);

    NSArray *pidArray = [[NSSet setWithArray:[
            (id)windowList
            valueForKey:@"kCGWindowOwnerPID"]]
            allObjects];

    if (windowList) {
        CFRelease(windowList);
    }

    __block std::vector<int> vectorList;
    vectorList.reserve([pidArray count]);
    [pidArray enumerateObjectsUsingBlock:^(NSNumber * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        vectorList.push_back([obj intValue]);
    }];

    return vectorList;
}
