   M S B A M L   `   `   `  ����N  JFirstFloor.ModernUI, Version=1.0.3.0, Culture=neutral, PublicKeyToken=null[2clr-namespace:FirstFloor.ModernUI.Windows.Controls$FirstFloor.ModernUI.Windows.Controls  5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089b ^Microsoft.Windows.Shell, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35  �� MergedDictionaries �� Source7    1FirstFloor.ModernUI.Windows.Controls.ModernWindow �� 
TargetType �� Property   WindowBackgroundContent �� Value  DefaultFontFamily  DefaultFontSize  
WindowText&   System.Windows.Media.TextOptions �� 
TargetType ��Name/  )Microsoft.Windows.Shell.SystemParameters2   Current �� Source	 �� Path  WindowBackgroundColor
 �� Color � 	Resources  Key �� 
TargetName �� TargetProperty	 �� To g� Duration  WindowBorder �Name � RowDefinitions    BackgroundContent �� Orientation*  $Microsoft.Windows.Shell.WindowChrome   IsHitTestVisibleInChrome � ToolTip  SystemButton,  &Microsoft.Windows.Shell.SystemCommands   MinimizeWindowCommand �� RelativeSource �� Mode �� AncestorType ��Name   RestoreWindowCommand � UseLayoutRounding   MaximizeWindowCommand   CloseWindowCommand � Title 
 Accent     
TitleLinks! ��Name	" �� X1	# �� Y1	$ �� X2	% �� Y2 	 SeparatorBackground 
 SystemButtonLink& �� ElementName' �� Triggers( �� Binding) �� Value* �� 
TargetName5   /FirstFloor.ModernUI.Windows.Controls.ModernMenu+    MenuLinkGroups,   
LinkGroups-   SelectedSource. �� Mode7   1FirstFloor.ModernUI.Windows.Controls.ModernButton/   IconData6   0FirstFloor.ModernUI.Windows.Controls.ModernFrame0  Name1    ContentLoader2   ContentLoader3   Source4 F�Name5 �� Stretch  WindowBorderActive   BooleanToVisibilityConverter6 �� 	Converter7 �� Triggers8 P� Property9 P� Value: ]� 
Conditions; �� Property< �� Value=   CornerRadius>   GlassFrameThickness?   UseAeroCaptionButtons�� 5      D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    :x,http://schemas.microsoft.com/winfx/2006/xaml     5      ?controls2clr-namespace:FirstFloor.ModernUI.Windows.Controls  5      Kshell?http://schemas.microsoft.com/winfx/2006/xaml/presentation/shell  5        5      �� 5   
   $7 1/FirstFloor.ModernUI;component/Assets/Button.xaml?�6   �� 5   
   $; 5/FirstFloor.ModernUI;component/Assets/Converters.xaml?�6   %�  '         7  7  7  7  7 
 7  �� 5      "   6   .�5   
   ��  �@  BackgroundContent# �   6.   �� 5   
    � ��# �  6'   �� 5   
    � ��# �  6%   �� 5   
    � ��# �  6'   �� 5   
    � ��$	 800��6"   �� 5   
    � ��$	 640��6#   �� 5   
    � ��$	 320��6%   �� 5   
    � ��$	 320��6&   �� 5   
    �@�
ResizeMode$ CanResizeWithGrip=�6'   �� 5   
    �@�UseLayoutRounding . 6.   �� 5   
    �@ TextFormattingMode$ Display=�6;   �� 5   
    � �� 5      �� 5      "   6"   .R�5      �� $ WindowBorder��6   ��63   �� # Z $#	 WindowNonClientFrameThickness���5      �� 5      WindowBorderBackground��#
 �  6N    5      ���� 5       BackgroundAnimation .��5!   "   �� $ WindowBorderBackground��61   $ Color�6`   # �  6�   $ 0:0:.6D�6�   5"      5#      ._�5%      � .+�5&      �� #���  6%   $��1q�6R   ._�5'   "   �� .u�5(   &   � $ 
LayoutRoot��6+    5)   *   �� 5*   .   $��36 �6<   �� 5+   .   $
��Auto �6<   �� 5,   .   $
��Auto �6<   �� 5-   .   $��* �6<   �� 5.   .   $��15 �6<   .+�52   *   �� $��5��69   #��z 6J   �� 5=   *   
������ $	��100��6J   $��30��6V   $��Right=�6b   $	��Top=�6~   $��
-50,0,76,0q�6�   ._�5>   .   �� $
��Fill=�65   $��90��6E   $��24��6P   $��Center=�6\   $��Center=�6y   $0��*pack://siteoforigin:,,,/Resources/logo.png��6�   5?   +   �� 5A   *   $ 
Horizontal=�65   $��Right=�6N   $	��Top=�6j   $��0,8,6,0q�6�    . .+�5B   .   �� $ minimize��6�   8��  ��65   �� # Z ��5C   2   � 5D   6   $��13��6;   $��12��6F   $z�1,0,0,1,0,1W�6R   .+�5E   :   F� ����    ��[ � z ��[ 	$��8��6R   $��7��6\   $��Center=�6g   $��Center=�6�   $��2��5F   �   ��6?   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5G   7   5I   /   � 5J   .   $��1,0,1,0q�63   .+�5K   2   �� $ Restore��69   $ restore��6�   8�� $y�	Collapsed=�6�   ��6J   �� # Z ��5L   6   � 5M   :   $��13��6?   $��12��6J    . $z�1,0,0,1,.5,.5W�6o   .+�5N   >   F� X���Ѐ�     � z     � z ��[ �    ��- р�[ ��- �    �� р�[ �� р�[  z �     z 	$��8��6y   $��8��6�   $��Center=�6�   $��Center=�6�   $��1��5O   �   ��6C   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5P   ;   5R   3   �� 5S   2   $ Maximize��69   $ maximize��6�   8�� ��6K   �� # Z ��5T   6   � 5U   :   $��13��6?   $��12��6J   .+�5V   >   F� +����    @B �@T� @B �@T�  z �     z 	$��9��6`   $��8��6j   $��Center=�6u   $��Center=�6�   $��2��5W   �   ��6C   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5X   ;   5Z   3   5[   /   �� 5\   .   $ close��6�   8�� ��65   �� # Z ��5]   2   � 5^   6   $��13��6;   $��12��6F   $z�1,0,0,1,0,1W�6R   .+�5_   :   F� +����        � z ��j � z     �    ��j 	$��8��6\   $��7��6f   $��Center=�6q   $��Center=�6�   $	��1.5��5`   �   ��6?   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5a   7   5c   /   5d   +   �� 5g   *   $ Horizontal =�65   $��Right=�6P   .+�5h   .   �� #��z 6:   $��9(�6Y   $��5,7,5,0q�6f   #���  6w   $	��Top=�6�   6�   �� 5i   .   #��z  6;   $��Right=�6f   $	��Top=�6�   $��	0,6,190,0q�6�    . ��5j   2   �� 5k   6   .R�5l   :   �� $ 
Horizontal=�6E   5m   7   ��5o   2   �� 5p   6   .R�5q   :   �� $ 
Horizontal=�6E   .+�5r   >   �� $! 	Separator��6C   $" .5��6V   $# 3��6^   $$ .5��6e   $% 12��6m   $��7,0,5,0q�6u   #��� 	 6�   �� 5t   >   $!��NavigationCommands.GoToPage��5u   A   8�� ��5t   E   �� *DisplayName+��5v   A   �� *Source+��5w   A   �� $& ContentFrame��5x   A   5y   ;   ' 5z   :   �� 5{   >   ( 6J   ��  �� *PreviousData+) 6�   Z� .a�5|   B   �� $* 	Separator��6_    � y�$ 	Collapsed=�6v   5}   ?   5   7   5�   /   5�   +     5�   *   $��1��6>   $��36,0,0,0q�5�   >   #, z+ 5�   >   - 5�   >   �� *Source+$& ContentFrame��$. TwoWay=�5�   >     5�   *   a/ ��@����O�@������p\-�������@��� �8@����ا  6n� �8��� ������� �p\-��O�@����O$#��NavigationCommands.BrowseBack��5�   @   $
 back��5�   @   $��2��5�   @   $
��Left=�6Q   $	��Top=�6l   $��	34,24,0,0q�6�    . ��5�   @   �� $& ContentFrame��5�   +     5�   *   $0 ContentFrame��6?   $��3��6U   $��2��6b   $��
36,8,16,16q�6s   #2 z1 6�   3 6�   �� *ContentSource+ �� *TemplatedParent+$. TwoWay=�6�   � 5�   *   $��4��6/   .+�5�   .   F� $4 
ResizeGrip��63   $y�	Collapsed=�6G   $��12��6^   $��12��6i   $��1q�6u   $��Right=�6�   #���  5�   3   $��1��5�   3   $
5 None=�5�   3   �����@B ��� ���- ��� �@KL ��� ���j ��� �@T� ��� ��ا ��� Ѐ� @T� р� �ا Ѐ�[ @T� р�[ �ا Ѐ�� @T� р�� �ا �@KL ��[ ���j ��[ �@T� ��[ ��ا ��[ Ѐ�[ @KL р�[ ��j Ѐ�� @KL р�� ��j �@T� �� ��ا �� Ѐ�� @B р�� ��- 5�   3   5�   +   5�   '   5�   #   5�      �� 5�      #���  6%   $��1q�6X   y�6l   �� *
IsActive+ �� *FindAncestor+" �86  5�      5�      7 5�      P� 5�      8 �@�WindowState$9 	Maximized=�69   . �5�      �� $* Maximize��6%    � y�$ 	Collapsed=�6Q   �� 5�      $* Restore��6%    � y�$ Visible=�6P   �� 5�      $* 
LayoutRoot��6%    � ��$ 7q�6O   5�      P� 5�      8 �@�WindowState$9 Normal=�69   . �5�      �� $* Maximize��6%    � y�$ Visible=�6Q   �� 5�      $* Restore��6%    � y�$ 	Collapsed=�6P   5�      ]� 5�      : 5�      �� 5�   "   ; �@�
ResizeMode$< CanResizeWithGrip=�6B   �� 5�   "   ; �@�WindowState$< Normal=�6C   ./�5�      �� $* 
ResizeGrip��6%    � y�$ Visible=�6S   5�      5�      5�      �� 5�   
    �@ WindowChrome 5�        5�      $= 0��6%   $> 1q�66   ? .  6N   5�      5�      5�      