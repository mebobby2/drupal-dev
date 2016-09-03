<?php 
// $Id$

?>

<div class="<?php print $classes; ?>">

  <div class="date"><?php print $date; ?>:</div>

  <h4><?php print $title_attributes; ?>
    <?php print render($title); ?>
  </h4>

  <div class="name">
    <?php print t('by !username', array('!username' => $name)); ?>
  </div>
  
</div>