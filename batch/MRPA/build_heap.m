function [heaps, heapind,invind] = build_heap(x)
%invind is current pos of 1:l point in heaps
if nargout>2
    b_inv = 1;
else
    b_inv = 0;
end

l = length(x);

heaps = x;
heapind = 1:l;
invind = 1:l;

for i = 2:l
    curnode = i;
    while (curnode>1)
        parnode = fix(curnode/2);
        if heaps(parnode)>heaps(curnode)%FOR MIN-heaps
            %swap value
            tmp = heaps(parnode);
            heaps(parnode) = heaps(curnode);
            heaps(curnode) = tmp;
            %swap heapind
            tmp = heapind(parnode);
            heapind(parnode) = heapind(curnode);
            heapind(curnode) = tmp;
            if b_inv
                %update invind
                invind(heapind(parnode))= parnode;
                invind(heapind(curnode))= curnode;
            end
            curnode = parnode;
        else
            break;
        end
    end
end



