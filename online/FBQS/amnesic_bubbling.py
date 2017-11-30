from qs_p_to_seg import *
import pickle
import sys
import os
import time
# 50KB, 12B per sample

MAX_BUFFER_LEN=1000
rcParams['pdf.use14corefonts'] = True
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = 'Helvetica'
rcParams['ps.useafm'] = True
rcParams['axes.unicode_minus'] = False

rcParams['xtick.labelsize'] = '24'
rcParams['ytick.labelsize'] = '24'
rcParams['legend.fontsize'] = '22'
rcParams['legend.frameon'] = False
rcParams['axes.labelsize'] = '24'
rcParams['xtick.major.pad'] = '24'

class BqsStorage(object):
    def __init__(self, max_len=MAX_BUFFER_LEN, nthresh = 2, \
        error_base = 2, error_multiplier = 5):

        self.storage = np.zeros([max_len,3])
        self.indices = np.zeros([0,3]).astype('int32')
        self.nthresh = nthresh
        self.error_base = error_base
        self.error_multiplier = error_multiplier
        self.max_len=max_len


    def compress(self, src_s, src_e, dest_s, age, buff_s=0):

        #print locals()

        if src_e > src_s:
            X = self.storage[src_s:src_e+1,:]
            #print self.storage.shape, '1', X.shape
        else:
            X = np.vstack( [self.storage[src_s:self.max_len,:], self.storage[buff_s:src_e+1,:]])
            #print self.storage.shape, '2', X.shape
            #print X

        prev_error = (self.error_multiplier**age)*self.error_base

        #print 'cur_err', (self.error_multiplier**age)*e
       
        res1, bounds, XX, arr1 = run_one(None, ((self.error_multiplier)**(age+1))*self.error_base, \
            check=False, approx=True, visualise=False, test=False, \
                X=X[:,:2], n=X.shape[0], prev_err=prev_error)

        

        #if age == 0:
            #print X.shape[0], len(arr1), X.shape[0]/len(arr1)
        #    nr = int(self.nthresh*0.9+X.shape[0]/len(arr1)*0.1)
        #    self.nthresh = self.nthresh if nr<self.nthresh else nr

        #    print 'nthresh', self.nthresh

        #self.storage[dest_s-len(arr1)+1:dest_s+1] = X[arr1[::-1],:]

        self.storage[dest_s:dest_s+len(arr1)] = X[arr1,:]

        self.update_index(age, -1,-1)
        self.update_index(age+1, dest_s, dest_s+len(arr1)-1)

        #print self.indices



    def update_index(self, age, start, end):

        if self.indices.shape[0] > 0:
            idx = np.argwhere(self.indices[:,0]==age)
            idx = -1 if len(idx)==0 else idx [0][0]

            #existing
            if idx != -1:

                #delete
                if start==end and start==-1:
                    self.indices = np.vstack([self.indices[:idx,:], self.indices[idx+1:,:]])
                #update existing
                else:

                    old_s, old_e = self.indices[idx][1:]

                    #merge
                    if old_e == start - 1 or start==-1:
                        self.indices[idx][2] = end
                    elif old_s -1 == end or end ==-1:
                        self.indices[idx][1] = start
                    else:
                        #print 'changing indices for %d: %s -> %s' % (age, (old_s,old_e), (start,end))
                        self.indices[age][1:] = (start,end)

            #insert new
            else:
                idx = np.argwhere(self.indices[:,0]<age)
                idx = -1 if len(idx)==0 else idx [-1][0] 

                if idx == -1:
                    self.indices = np.vstack([[age,start,end],self.indices])
                else:
                    self.indices = np.vstack([self.indices[:idx+1,:], [age,start,end], self.indices[idx+1:,:]])

        else:
            self.indices = np.vstack([self.indices, [age,start,end]])                


    def trigger(self):

        if len(self.indices)>0:
            # 1, storage full 2,cyclic storage full 3,nthresh reached
            #if self.indices[0][0] == 0:

            if self.indices[0][2] == self.max_len-1:
                    return 1

            #tmp = np.ceil(self.nthresh*(1.0/(1+e**np.arange(0,self.indices[0][0]+1)))) 
            #tmp[tmp<2]=2

            #tmp = np.ceil(self.nthresh*(0.25**np.arange(0,self.indices[0][0]+1)))
            #gap = np.sum(tmp)
            #gap = self.nthresh*(self.indices[0][0]+1)
            gap = self.nthresh + self.indices[0][0]

            #gap = self.nthresh 

            #print 'gap %d=>%d'%(self.indices[0][0],gap)

            if self.indices[0][0] > 0 and \
                self.indices[0][2] > self.max_len - gap-1:
                #print 'gap %d=>%d'%(self.indices[0][0],gap)

                return 2
        return 0
    # def 

    def add_point(self, p):

        if self.indices.shape[0]>0:
            next_available = self.indices[0][2] + 1

            if self.indices[0][0] == 0:
                if next_available == self.max_len and self.indices.shape[0]>1 \
                    and self.indices[1][2]<self.indices[0][1]-1:
                    next_available = self.indices[1][2]+1

                    #print 'AAAAAAAAA'

                self.update_index(0, -1, next_available)
            else:
                
                self.update_index(0, next_available, next_available)
            #print next_available
        else:
            next_available = 0
            self.update_index(0, next_available, next_available)

        self.storage[next_available] = p
    
        self.bubble()


    def bubble(self):


        flag = self.trigger()

        while flag > 0:
            #print 'flag',flag
            buff_s = 0 if self.indices.shape[0]==1 else self.indices[1][2]+1

            self.compress(self.indices[0][1], self.indices[0][2], \
                buff_s, self.indices[0][0], buff_s)
            
            #print self.indices
            flag = self.trigger()
            #break


def test_compress():
    global MAX_BUFFER_LEN
    MAX_LEN=MAX_BUFFER_LEN
    XX, n = merge_all_data(ids=[163])

    

    XX = XX[:MAX_LEN,:]
    XX=XX[::-1]
    N = MAX_LEN


    e = 2
    
    XX = np.zeros([10,2])
    XX[:,0]=range(10)
    XX=XX[::-1,:]
    MAX_LEN = 10
    obj = BqsStorage(error_base=e)
    obj.storage = np.copy(XX)

    print obj.storage.shape
    obj.compress(MAX_LEN/2-1, MAX_LEN/2, MAX_LEN-1, 0, MAX_LEN-1)

    XX=XX[::-1]
    XXX = np.vstack( [XX[MAX_LEN/2:MAX_LEN,:],XX[:MAX_LEN/2,:]] )
    print 'xxxx',XXX.shape
    print XXX
    res, bounds, X, arr = run_one(None, e, check=False, approx=False, visualise=False, test=False, X=XXX, n=XX.shape[0])
    y = XXX[arr,:]
    yy = obj.storage[MAX_LEN-len(arr):MAX_LEN,:][::-1]


    print y[:10,:]
    print yy[:10,:]

    #print XX[]

    print np.sum(y==yy), y.shape, yy.shape



def test_update_index():
    global MAX_BUFFER_LEN
    MAX_LEN=MAX_BUFFER_LEN

    print MAX_LEN
    obj = BqsStorage()

    obj.update_index(0, MAX_LEN-1, 0)
    print 't1', obj.indices

    obj.update_index(3, MAX_LEN-1, MAX_LEN-128+1)
    
    #obj.update_index(0, -1, -1)
    print 't2', obj.indices


    obj.update_index(1, MAX_LEN-128, MAX_LEN-256+1)
    print 't3', obj.indices


    obj.update_index(0, MAX_LEN-256, -1)
    obj.update_index(3, -1, MAX_LEN-56+1)
    print 't4', obj.indices


    obj.update_index(2, MAX_LEN-56, MAX_LEN-128+1)
    print 't5', obj.indices



def test_trigger():
    global MAX_BUFFER_LEN
    MAX_LEN=MAX_BUFFER_LEN
    
    nthresh = 16
    obj = BqsStorage(nthresh=nthresh)
    obj.indices = np.array([
            [0, MAX_LEN-512, 0]
        ])
    assert( obj.trigger() == True )



    obj = BqsStorage(nthresh=nthresh)
    obj.indices = np.array([
            [0, MAX_LEN-256, MAX_LEN-255],
            [1, MAX_LEN-1, MAX_LEN-512]
        ])
    assert( obj.trigger() == True )


    obj = BqsStorage(nthresh=nthresh)
    obj.indices = np.array([
            [1, MAX_LEN-256, nthresh+2]
        ])
    assert( obj.trigger() == False )


    obj = BqsStorage(nthresh=nthresh)
    obj.indices = np.array([
            [2, MAX_LEN-256, nthresh+2]
        ])
    assert( obj.trigger() == False )


    obj = BqsStorage(nthresh=nthresh)
    obj.indices = np.array([
            [2, MAX_LEN-256, nthresh+1]
        ])
    assert( obj.trigger() == True )

def decay(ages):
    weights = np.linspace(1.0,.2, len(ages))
    return weights

def test_add(X, err,  max_len=MAX_BUFFER_LEN):

    #X, n = merge_all_data(ids=[171])
    res, bounds, XXX, arr = run_one(None, err, check=False, approx=True, visualise=False, test=False, X=X, n=X.shape[0])
    XX = np.vstack([X[arr].T, arr]).T
    #print arr

    rn = int(n/len(arr))

    if rn > max_len/3:
        rn = int(max_len/3)
    #print 'ratio', rn
    
    obj = BqsStorage(nthresh=rn, error_multiplier=2.5, error_base=err, max_len=max_len)

    
    #X, n = merge_all_data(ids=[171])

    #XX = np.vstack([X.T, np.arange(X.shape[0])]).T

    for i in range(XX.shape[0]):

        #if i>=100 and i%100==0:
        #    print '%dth point'% i
        p = XX[i,:]
        obj.add_point(p)

    #print obj.indices
    #print obj.storage[obj.indices[0,2]:,:]

    #print obj.storage[obj.indices[0,2]:,2]

    #print obj.storage[ :obj.indices[0,2]+1,2]

    #print np.sum(obj.storage[ 1:obj.indices[0,2]+1,2]< obj.storage[ 0:obj.indices[0,2],2])


    ids = np.int32(obj.storage[ :obj.indices[0,2]+1,2])
    return ids


    #return  [sync_error(obj.storage[ :obj.indices[0,2]+1,:], X, age_decay_func=None, plot=False), sync_error(obj.storage[ :obj.indices[0,2]+1,:], X, age_decay_func=decay, plot=False), curve_approx_dist(X[:,0],X[:,1],ids)[0]]

    #print 'std', np.std(X, axis=0)


def test_bqs(XX, e, max_len=MAX_BUFFER_LEN):
    
#    XX, n = merge_all_data(ids=[171,174,163,171,174,163])
    #XX, n = merge_all_data(ids=[171])
    #XX, n = merge_all_data(ids=[171])

    res, bounds, X, arr = run_one(None, e, check=False, approx=True, visualise=False, test=False, X=XX, n=XX.shape[0])
    #print arr

    arr=np.array(arr)
    #print len(arr)
    #print arr
    if len(arr) > max_len:
        print len(arr)-max_len
        arr = arr[len(arr)-max_len:]
        arr = np.hstack([[0],arr])
        #print arr
    return arr
    #return [sync_error(np.vstack( [XX[arr].T, arr] ).T, XX, age_decay_func=None, plot=False), sync_error(np.vstack( [XX[arr].T, arr] ).T, XX, age_decay_func=decay, plot=False), curve_approx_dist(XX[:,0],XX[:,1],arr)[0]]
    #print 'std', np.std(XX, axis=0)

def test_dp(XX, e, max_len=MAX_BUFFER_LEN):
    
#    XX, n = merge_all_data(ids=[171,174,163,171,174,163,171,174,163,171,174,163])
    #XX, n = merge_all_data(ids=[171])
    n = XX.shape[0]
    arr, dmax=DouglasPeucker(XX, range(n), e)
    #print arr

    arr=np.array(arr)
    #print len(arr)
    if len(arr) > max_len:
        #print len(arr)-max_len
        arr = arr[len(arr)-max_len:]
        arr = np.hstack([[0],arr])
        #print arr
    return [sync_error( np.vstack( [XX[arr].T, arr] ).T, XX, age_decay_func=None, plot=False), sync_error( np.vstack( [XX[arr].T, arr] ).T, XX, age_decay_func=decay, plot=False), curve_approx_dist(X[:,0],X[:,1],arr)[0]]
    #print 'std', np.std(XX, axis=0)


styles = ['k-.^','k-.s','k-.s','k-.+','k-.+','k-.o','k-.o','.-*'] 



def get_all_results(X, num_points):

    

    res = np.zeros([0,12])

    for err in np.arange(10,101,10):
        for r in [0.005, 0.01,0.02,0.03,0.04,0.05]:
    # for err in np.arange(10,60,50):
    #     for r in [0.01]:
        
            for i in num_points:
                max_len = i*r
                print 'no points %d, storage size %d' %(i,max_len)
                XX = X[:i,:]
                res = np.vstack([
                    res,
                    [err,r,i]+
                    test_add(XX, err, max_len)[0]+
                    test_bqs(XX, err, max_len)[0]+
                    test_dp(XX, err, max_len)[0]
                ])


    import pickle
    pickle.dump(res, open('../data/results_all.p','wb'))




def plot_err():

    import pickle
    R = pickle.load(open('../data/results_all.p','rb'))

    #for err in np.arange(10,101,10):
    #    for r in [0.005, 0.01,0.02,0.03,0.04,0.05]:
    
    for n in [80000]:
         for r in [0.03]:

            idx = np.logical_and(R[:,1]==r, R[:,2]==n)
            
            errs = np.int_(R[idx,0])[:-2] 
            print errs
            res = R[idx, 3:][:-2,:]


            
            # res = np.zeros([0,9])
            # for i in num_points:
            #     max_len = i*r
            #     print 'no points %d, storage size %d' %(i,max_len)
            #     XX = X[:i,:]
            #     res = np.vstack([
            #         res,
            #         test_add(XX, err, max_len)+
            #         test_bqs(XX, err, max_len)+
            #         test_dp(XX, err, max_len)
            #     ])


            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( errs, res[:,i*3], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error (m)')
            ax.set_xlabel('Error Tolerance (m)')
            ax.set_xticks(errs)
            ax.set_xticklabels( errs )
            ax.set_yscale('log')
            plt.savefig('../figs/tol-%d-%d.pdf'%(i,100*r), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( errs, res[:,i*3+1], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error with Decay (m)')
            ax.set_xlabel('Error Tolerance (m)')
            ax.set_xticks(errs)
            ax.set_xticklabels( errs )
            ax.set_yscale('log')
            plt.savefig('../figs/told-%d-%d.pdf'%(i,100*r), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( errs, res[:,i*3+2], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Deviation (m)')
            ax.set_xlabel('Error Tolerance (m)')
            ax.set_xticks(errs)
            ax.set_xticklabels( errs )
            ax.set_yscale('log')
            plt.savefig('../figs/tolm-%d-%d.pdf'%(i,100*r), bbox_inches='tight')
            plt.close()


def plot_r():

    import pickle
    R = pickle.load(open('../data/results_all.p','rb'))

    #for err in np.arange(10,101,10):
    #    for r in [0.005, 0.01,0.02,0.03,0.04,0.05]:
    
    for n in [80000]:
         for err in [20]:

            idx = np.logical_and(R[:,0]==err, R[:,2]==n)
            
            rs = R[idx,1]
            print rs
            res = R[idx, 3:]


            
            # res = np.zeros([0,9])
            # for i in num_points:
            #     max_len = i*r
            #     print 'no points %d, storage size %d' %(i,max_len)
            #     XX = X[:i,:]
            #     res = np.vstack([
            #         res,
            #         test_add(XX, err, max_len)+
            #         test_bqs(XX, err, max_len)+
            #         test_dp(XX, err, max_len)
            #     ])


            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( 100*rs, res[:,i*3], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error (m)')
            ax.set_xlabel('Storage/data Ratio (%)')
            ax.set_xticks(100*rs)
            ax.set_xticklabels( 100*rs )
            ax.set_yscale('log')
            plt.savefig('../figs/ratio-%d-%d.pdf'%(n,err), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( 100*rs, res[:,i*3+1], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error with Decay (m)')
            ax.set_xlabel('Storage/data Ratio (%)')
            ax.set_xticks(100*rs)
            ax.set_xticklabels( 100*rs )
            ax.set_yscale('log')
            plt.savefig('../figs/ratiod-%d-%d.pdf'%(n,err), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( 100*rs, res[:,i*3+2], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Deviation (m)')
            ax.set_xlabel('Storage/data Ratio (%)')
            ax.set_xticks(100*rs)
            ax.set_xticklabels( 100*rs )
            ax.set_yscale('log')
            plt.savefig('../figs/ratiom-%d-%d.pdf'%(n,err), bbox_inches='tight')
            plt.close()



def moving_average(a, n=3) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n


def plot_error_decay(X,n):
    # err = 20
    # r = 0.03
    # max_len = int(n*r)
                    
    # XX = X[:n,:]
    # e0 = moving_average(test_add(XX, err, max_len)[0][1][::-1], 10000)
    # e1 = moving_average(test_bqs(XX, err, max_len)[0][1][::-1], 10000)
    # e2 = moving_average(test_dp(XX, err, max_len)[0][1][::-1], 10000)
    # res = np.vstack([e0,e1,e2]).T

    # pickle.dump(res, open('../data/err_decay.p','wb'))
    res = pickle.load(open('../data/err_decay.p','rb'))

    x = range(res.shape[0])
    fig, ax = plt.subplots()    

    lines = []
    lstyles = ['-','--',':']
    for i in range(3):
        l, = ax.plot( x, res[:,i], lstyles[i] )
        lines.append(l)

    means = np.mean(res, axis=0)
    ax.legend(lines, [r'ABQS, $\mu=%d$'%means[0],r'FBQS, $\mu=%d$'%means[1],r'DP, $\mu=%d$'%means[2]], loc=0)
    plt.tight_layout()
    ax.set_ylabel('Time-synchronized Error (m)')
    ax.set_xlabel('Index of point (thousand)')
    ax.set_xticks(range(0,80000, 10000))
    ax.set_xticklabels( range(0,80,10) )
    ax.set_yscale('log')
    plt.savefig('../figs/sync_error.pdf', bbox_inches='tight')
    plt.close()
    




def plot_nop(num_points):

    import pickle
    R = pickle.load(open('../data/results_all.p','rb'))

    #for err in np.arange(10,101,10):
    #    for r in [0.005, 0.01,0.02,0.03,0.04,0.05]:
    
    for err in [20]:
         for r in [0.03]:

            idx = np.logical_and(R[:,0]==err, R[:,1]==r)
            
            print R[idx,2]
            res = R[idx, 3:]


            
            # res = np.zeros([0,9])
            # for i in num_points:
            #     max_len = i*r
            #     print 'no points %d, storage size %d' %(i,max_len)
            #     XX = X[:i,:]
            #     res = np.vstack([
            #         res,
            #         test_add(XX, err, max_len)+
            #         test_bqs(XX, err, max_len)+
            #         test_dp(XX, err, max_len)
            #     ])


            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( R[idx,2]/1000, res[:,i*3], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error (m)')
            ax.set_xlabel('No. Points (thousand)')
            ax.set_yscale('log')
            ax.set_xticks(R[idx,2]/1000)
            ax.set_xticklabels( np.int_(R[idx,2]/1000) )
            plt.savefig('../figs/nop-%d-%d.pdf'%(err,100*r), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( R[idx,2]/1000, res[:,i*3+1], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Time-synchronized Error with Decay (m)')
            ax.set_xlabel('No. Points (thousand)')
            ax.set_xticks(R[idx,2]/1000)
            ax.set_xticklabels( np.int_(R[idx,2]/1000) )
            ax.set_yscale('log')
            plt.savefig('../figs/nopd-%d-%d.pdf'%(err,100*r), bbox_inches='tight')
            plt.close()

            fig, ax = plt.subplots()    
            lines = []
            for i in range(3):
                l, = ax.plot( R[idx,2]/1000, res[:,i*3+2], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
                lines.append(l)
            ax.legend(lines, ['ABQS','FBQS','DP'], loc=0)
            plt.tight_layout()
            ax.set_ylabel('Deviation (m)')
            ax.set_xlabel('No. Points (thousand)')
            ax.set_xticks(R[idx,2]/1000)
            ax.set_xticklabels( np.int_(R[idx,2]/1000) )
            ax.set_yscale('log')
            plt.savefig('../figs/nopm-%d-%d.pdf'%(err,100*r), bbox_inches='tight')
            plt.close()





# def fix_storage_ratio(X, num_points):

#     print num_points
#     #max_len = int(max(num_points)*0.05)

#     #import pickle
#     #R = pickle.load(open('../data/results_all.p','rb'))

#     for err in [20]:

#         for r in [0.05]:
    

#             # idx = np.logical_and(R[:,0]==err, R[:,1]==r)
#             # res = R[np.argsort(R[idx,2]), 3:]
#             res = np.zeros([0,9])
#             for i in num_points:
#                 max_len = i*r
#                 print 'no points %d, storage size %d' %(i,max_len)
#                 XX = X[:i,:]
#                 res = np.vstack([
#                     res,
#                     test_add(XX, err, max_len)+
#                     test_bqs(XX, err, max_len)+
#                     test_dp(XX, err, max_len)
#                 ])


#             fig, ax = plt.subplots()    
#             lines = []
#             for i in range(3):
#                 l, = ax.plot( num_points/1000, res[:,i*3], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
#                 lines.append(l)
#             ax.legend(lines, ['ABQS','FBQS','DP'])
#             plt.tight_layout()
#             ax.set_ylabel('Time-synchronized Error (m)')
#             ax.set_xlabel('No. Points (thousand)')
#             ax.set_xticks(num_points/1000)
#             ax.set_xticklabels( num_points/1000 )
#             plt.savefig('../figs/abqs-fixr-%d-%.3f.pdf'%(err,r), bbox_inches='tight')
#             plt.close()

#             fig, ax = plt.subplots()    
#             lines = []
#             for i in range(3):
#                 l, = ax.plot( num_points/1000, res[:,i*3+1], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
#                 lines.append(l)
#             ax.legend(lines, ['ABQS','FBQS','DP'])
#             plt.tight_layout()
#             ax.set_ylabel('Time-synchronized Error with Decay (m)')
#             ax.set_xlabel('No. Points (thousand)')
#             ax.set_xticks(num_points/1000)
#             ax.set_xticklabels( num_points/1000 )
#             plt.savefig('../figs/abqs-fixrd-%d-%.3f.pdf'%(err,r), bbox_inches='tight')
#             plt.close()

#             fig, ax = plt.subplots()    
#             lines = []
#             for i in range(3):
#                 l, = ax.plot( num_points/1000, res[:,i*3+2], styles[i*2], markersize=14, markerfacecolor='w', linewidth=3 )
#                 lines.append(l)
#             ax.legend(lines, ['ABQS','FBQS','DP'])
#             plt.tight_layout()
#             ax.set_ylabel('Deviation (m)')
#             ax.set_xlabel('No. Points (thousand)')
#             ax.set_xticks(num_points/1000)
#             ax.set_xticklabels( num_points/1000 )
#             ax.set_yscale('log')
#             plt.savefig('../figs/abqs-fixm-%d-%.3f.pdf'%(err,r), bbox_inches='tight')
#             plt.close()

def get_save_path(path,filename,epsion):
    if not os.path.exists(path):
        os.mkdir(path)
    if "geolife" in filename:
        save_path = path+'geolife/'
    elif "SingaporeTaxi" in filename:
        save_path = path + 'SingaporeTaxi/'
    elif "truck" in filename:
        save_path = path + 'truck/'
    elif "game" in filename:
        save_path = path +"game/"
    elif "indoor" in filename:
        save_path = path +"indoor/"
    elif "Illinois" in filename:
        save_path = path + "Illinois/"
    if not os.path.exists(save_path):
        os.mkdir(save_path)
    save_path = save_path + str(epsion)+'/'
    if not os.path.exists(save_path):
        os.mkdir(save_path)
    save_path = save_path + filename.split('/')[-3]+'/'
    if not os.path.exists(save_path):
        os.mkdir(save_path)
    save_path = save_path + filename.split('/')[-2]+'/'
    if not os.path.exists(save_path):
        os.mkdir(save_path)
    return save_path

def scan_files(directory,prefix=None,postfix=None):
    files_list=[]
    for root, sub_dirs, files in os.walk(directory):
        for special_file in files:
            if postfix:
                if special_file.endswith(postfix):
                    files_list.append(os.path.join(root,special_file))
            elif prefix:
                if special_file.startswith(prefix):
                    files_list.append(os.path.join(root,special_file))
            else:
                files_list.append(os.path.join(root,special_file))
                             
    return files_list

def save_points(save_path,filename,result_points,time_consume):
        fout = open(save_path + filename.split('/')[-1],'w')
        #fout.write(str(time_consume)+'\n')
        for i in result_points:
                fout.write(i)
        fout.write(str(time_consume)+'\n')
        fout.close()

def gpsreader(path):
    # 41.475306 -88.066666 68994
    points = []
    f = open(path,'r')
    lines = f.readlines()
    for line in lines:
        temp_line = line.split(" ")
        points.append([float(temp_line[0]),float(temp_line[1])])
    points = np.array(points)
    return points,points.shape[0],lines

def save_points(save_path,filename,result_points,time_consume):
        fout = open(save_path + filename.split('/')[-1],'w')
        for i in result_points:
                fout.write(i)
        fout.write(str(time_consume)+'\n')
        fout.close()

if __name__ == '__main__':
    # test_compress()
    # test_update_index()
    # test_trigger()
 
    #X, n = merge_all_data(ids=[171,174,163,171,174,163,171,174])
    #X,n = merge_all_data(ids = [175])
    #print X,n
    #n=80000
    #X=X[:n,:]
    #print np.std(X, axis=0)
    #X, n = merge_all_data(ids=[174,174])
    #print n

    #fix_storage_ratio(range(10000,80001,10000))

    #test_dp(X, 20, 100)
    paras = sys.argv
    filename = paras[1]
    epsion= float(paras[2])
    X,n,rawlines = gpsreader(filename)
    ids = test_bqs(X, epsion, int(n))
    print filename
    print ids
    # test_bqs()

    #fix_storage_ratio(X,range(10000,80001,10000))
    #plot_nop(range(10000,80001,10000))
    #plot_err()
    #plot_r()
    #plot_error_decay(X, 80000)

    #get_all_results(X, range(10000,20001,10000))
    #get_all_results(X, range(10000,80001,10000))

